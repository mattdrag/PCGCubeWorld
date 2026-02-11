
#include "Pawn/ZXPawn.h"

#include "Camera/CameraComponent.h"
#include "Pawn/ZXPawnMovementComponent.h"
#include "World/GridManagerComponent.h"
#include "Core/ZXUtils.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AZXPawn::AZXPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root:
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	// Scene components:
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComponent->SetupAttachment(RootComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	// additional components:
	MovementComponent = CreateDefaultSubobject<UZXPawnMovementComponent>(TEXT("MovementComponent"));
}

// Called when the game starts or when spawned
void AZXPawn::BeginPlay()
{
	Super::BeginPlay();

	UGridManagerComponent* GridManager = UZXUtils::GetGridManager(this);
	if (!IsValid(GridManager))
	{
		LOGZXEF("Invalid GridManager..");
		return;
	}
	CachedGridManager = GridManager;
	
	// reserve some space in our arrays:
	LoadedCubes.Reserve(CubeLoadRange.X * CubeLoadRange.Y);
	CubeBuffer_Load.Reserve(CubeLoadRange.X * CubeLoadRange.Y);
	CubeBuffer_Unload.Reserve(CubeLoadRange.X * CubeLoadRange.Y);
	
	// start at zero vec:
	SetActorRotation(FRotator::ZeroRotator);
	SetActorLocation(FVector::ZeroVector + FVector(0.f, 0.f, GridManager->GetZHeight()));
	
	// Load once:
	LastGridLocation = GridManager->WorldToIndex(GetActorLocation());
	const FIntPoint CurrentCoord = GridManager->IndexToCoordinates(LastGridLocation);
	// Do a single blocking load:
	for (int32 x = -CubeLoadRange.X; x <= CubeLoadRange.X; x++)
	{
		for (int32 y = -CubeLoadRange.Y; y <= CubeLoadRange.Y; y++)
		{
			const int32 CubeIdx = GridManager->CoordinatesToIndex(CurrentCoord + FIntPoint(x, y));
			GridManager->SpawnCube(CubeIdx);
			LoadedCubes.Add(CubeIdx);
		}
	}
	LoadedCubeWindow = FBox2D(CurrentCoord + FIntPoint(CubeLoadRange.X, CubeLoadRange.Y)*-1, CurrentCoord + FIntPoint(CubeLoadRange.X, CubeLoadRange.Y));
}

// Called every frame
void AZXPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TRACE_CPUPROFILER_EVENT_SCOPE(AZXPawn_Tick);
	
	// for debugging:
	//LOGZX("LoadedCubes=[%d] | BufferLoad=[%d] | BufferUnload=[%d]", LoadedCubes.Num(), CubeBuffer_Load.Num(), CubeBuffer_Unload.Num());
	//LOGZX("%d,%d", GridManager->IndexToCoordinates(CurrentGridLocation).X, GridManager->IndexToCoordinates(CurrentGridLocation).Y);
	
	UGridManagerComponent* GridManager = CachedGridManager.Get();
	if (IsValid(GridManager))
	{
		const int32 CurrentGridLocation = GridManager->WorldToIndex(GetActorLocation());
		if (CurrentGridLocation != LastGridLocation)
		{
			// if we change locations, buffer some cubes to be loaded:
			BufferCubes(LastGridLocation, CurrentGridLocation);
			LastGridLocation = CurrentGridLocation;
		}
	
		// start by marking cubes:
		TArray<int32> LocalMarkedCubes;
		LocalMarkedCubes.Reserve(LoadedCubesPerFrame);
		if (!CubeBuffer_Unload.IsEmpty())
		{
			for (const int32 CubeIdx : CubeBuffer_Unload)
			{
				int32 i = 0;
				LocalMarkedCubes.Add(CubeIdx);
				i++;
				if (i >= LoadedCubesPerFrame)
				{
					break;
				}
			}
		}
		
		// if theres any cubes in the load buffer, load em:
		if (!CubeBuffer_Load.IsEmpty())
		{
			int32 i = 0;
			TArray<int32> LocalLoadedCubes;
			LocalLoadedCubes.Reserve(LoadedCubesPerFrame);
			for (const int32 CubeIdx : CubeBuffer_Load)
			{
				GridManager->SpawnCube(CubeIdx, LocalMarkedCubes[i]);
				LoadedCubes.Add(CubeIdx);
				LocalLoadedCubes.Add(CubeIdx);
				i++;
				if (i >= LoadedCubesPerFrame)
				{
					break;
				}
			}
			for (const int32 CubeIdx : LocalLoadedCubes)
			{
				CubeBuffer_Load.Remove(CubeIdx);
			}
		}
	
		// if theres any cubes in the unload buffer, unload em:
		if (!CubeBuffer_Unload.IsEmpty())
		{
			int32 i = 0;
			TArray<int32> LocalUnloadedCubes;
			LocalUnloadedCubes.Reserve(LoadedCubesPerFrame);
			for (const int32 CubeIdx : CubeBuffer_Unload)
			{
				GridManager->FreeCube(CubeIdx);
				LoadedCubes.Remove(CubeIdx);
				LocalUnloadedCubes.Add(CubeIdx);
				i++;
				if (i >= LoadedCubesPerFrame)
				{
					break;
				}
			}
			for (const int32 CubeIdx : LocalUnloadedCubes)
			{
				CubeBuffer_Unload.Remove(CubeIdx);
			}
		}
	}
}

void AZXPawn::BufferCubes(int32 OldLocation, int32 NewLocation)
{
	UGridManagerComponent* GridManager = CachedGridManager.Get();
	if (!IsValid(GridManager))
	{
		return;
	}
	
	// loading window slides to new location, get new box, then take the difference:
	const FIntPoint OldCoord = GridManager->IndexToCoordinates(OldLocation);
	const FIntPoint NewCoord = GridManager->IndexToCoordinates(NewLocation);
	const FIntPoint Diff = NewCoord - OldCoord;
	
	auto BufferOperation = [&](int32 XMin, int32 XMax, int32 YMin, int32 YMax, bool bLoad)
	{
		for (int32 x = XMin; x < XMax; x++)
		{
			for (int32 y = YMin; y < YMax; y++)
			{
				const int32 LoadIdx =  GridManager->CoordinatesToIndex(x, y);
				bLoad ? BufferLoad(LoadIdx) : BufferUnload(LoadIdx);
			}
		}
	};

	// shift right:
	if (Diff.Y > 0)
	{
		// unload:
		BufferOperation(LoadedCubeWindow.Min.X, LoadedCubeWindow.Max.X + 1, 
						LoadedCubeWindow.Min.Y, LoadedCubeWindow.Min.Y + Diff.Y, 
						false);
		// load:
		BufferOperation(LoadedCubeWindow.Min.X, LoadedCubeWindow.Max.X + 1, 
						LoadedCubeWindow.Max.Y + 1, LoadedCubeWindow.Max.Y + Diff.Y + 1, 
						true);
	}
	// shift left
	else if (Diff.Y < 0)
	{
		// unload:
		BufferOperation(LoadedCubeWindow.Min.X, LoadedCubeWindow.Max.X + 1, 
						LoadedCubeWindow.Max.Y + Diff.Y + 1, LoadedCubeWindow.Max.Y + 1, 
						false);
		// load:
		BufferOperation(LoadedCubeWindow.Min.X, LoadedCubeWindow.Max.X + 1, 
						LoadedCubeWindow.Min.Y + Diff.Y, LoadedCubeWindow.Min.Y, 
						true);
	}
	
	// slide the window on Y 
	// - if we slide both axis at once, the corners wont work properly.
	LoadedCubeWindow = LoadedCubeWindow.ShiftBy(FIntPoint(0, Diff.Y));
	
	// shift up:
	if (Diff.X > 0)
	{
		// unload:
		BufferOperation(LoadedCubeWindow.Min.X, LoadedCubeWindow.Min.X + Diff.X, 
						LoadedCubeWindow.Min.Y, LoadedCubeWindow.Max.Y + 1, 
						false);
		
		// load:
		BufferOperation(LoadedCubeWindow.Max.X + 1, LoadedCubeWindow.Max.X + Diff.X + 1, 
						LoadedCubeWindow.Min.Y, LoadedCubeWindow.Max.Y + 1, 
						true);
	}
	// shift down
	else if (Diff.X < 0)
	{
		// unload:
		BufferOperation(LoadedCubeWindow.Max.X + Diff.X + 1, LoadedCubeWindow.Max.X + 1, 
						LoadedCubeWindow.Min.Y, LoadedCubeWindow.Max.Y + 1, 
						false);
		
		// load:
		BufferOperation(LoadedCubeWindow.Min.X + Diff.X, LoadedCubeWindow.Min.X, 
						LoadedCubeWindow.Min.Y, LoadedCubeWindow.Max.Y + 1, 
						true);
	}
	
	// finish the window shift:
	LoadedCubeWindow = LoadedCubeWindow.ShiftBy(FIntPoint(Diff.X, 0));
}

void AZXPawn::BufferLoad(int32 InIdx)
{
	if (!LoadedCubes.Contains(InIdx))
	{
		CubeBuffer_Load.Add(InIdx);
	}
	CubeBuffer_Unload.Remove(InIdx);
}

void AZXPawn::BufferUnload(int32 InIdx)
{
	if (LoadedCubes.Contains(InIdx))
	{
		CubeBuffer_Unload.Add(InIdx);
	}
	CubeBuffer_Load.Remove(InIdx);
}