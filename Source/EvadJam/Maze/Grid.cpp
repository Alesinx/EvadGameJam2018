// Fill out your copyright notice in the Description page of Project Settings.

#include "Grid.h"
#include "Tile.h"
#include "Runtime/Engine/Classes/Engine/World.h"


// Sets default values
AGrid::AGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Size = 4;
	TileSize = 200;
	StartTileActor = nullptr;
	GoalTileActor = nullptr;
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();
}

//In this case we generate a maze using hunt and kill algorithm
void AGrid::GenerateMaze()
{
	ClearGrid();
	InitializeGrid();
	//Make a random path until reach a dead end
	VisitedTiles = TArray<FVector>();
	FVector StartPosition(1, 1, 0);
	FVector ActualPosition(1,1,0);
	FVector LastPosition;

	while (ActualPosition != FVector(-1, 0, 0)) {
		LastPosition = ActualPosition;
		WalkFrom(ActualPosition);
		ActualPosition = Hunt();
	}
	
	GoalTileActor = PlaceGoalTile(LastPosition);
}
void AGrid::WalkFrom(FVector StartPosition)
{
	bool DeadEndReached = false;

	FVector ActualPosition(StartPosition);
	FMovement NextMovement;

	FVector TileLocation = GridRows[StartPosition.X].Columns[StartPosition.Y]->GetActorLocation();
	GridRows[StartPosition.X].Columns[StartPosition.Y]->Destroy();
	if (StartTileActor == nullptr) {
		StartTileActor = PlaceStartTile(TileLocation);
	}
	else {
		PlaceWalkTile(TileLocation);
	}
	VisitedTiles.Add(ActualPosition);

	while (!DeadEndReached) {
		TArray<FMovement> PossibleMovements = GetPossibleMovementsFrom(ActualPosition);
		if (PossibleMovements.Num() > 0) {
			NextMovement = GetRandomMovementFrom(PossibleMovements);
			ApplyMovement(NextMovement);
			ActualPosition = NextMovement.Movement;
			VisitedTiles.Add(ActualPosition);
		}
		else {
			DeadEndReached = true;
		}
	}
}

void AGrid::ApplyMovement(FMovement Movement) {

	if (GridRows.IsValidIndex(Movement.IntermediateTile.X) && GridRows[Movement.IntermediateTile.X].Columns.IsValidIndex(Movement.IntermediateTile.Y)) {
		FVector TileLocation = GridRows[Movement.IntermediateTile.X].Columns[Movement.IntermediateTile.Y]->GetActorLocation();
		GridRows[Movement.IntermediateTile.X].Columns[Movement.IntermediateTile.Y]->Destroy();
		PlaceWalkTile(TileLocation);
	}
	if (GridRows.IsValidIndex(Movement.Movement.X) && GridRows[Movement.Movement.X].Columns.IsValidIndex(Movement.Movement.Y)) {
		FVector TileLocation = GridRows[Movement.Movement.X].Columns[Movement.Movement.Y]->GetActorLocation();
		GridRows[Movement.Movement.X].Columns[Movement.Movement.Y]->Destroy();
		PlaceWalkTile(TileLocation);
	}
}

void AGrid::InitializeGrid()
{
	UE_LOG(LogTemp, Warning, TEXT("INITIALIZING GRID START"));
	int32 GridSize = Size * 2 - 1;
	for (int32 i = 0; i < GridSize; i++) {
		GridRows.Add(FGridColumn());
		for (int32 j = 0; j < GridSize; j++) {
			GridRows[i].Columns.Add(PlaceWallTile(FVector(i*TileSize,j*TileSize,GetActorLocation().Z)));
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("INITIALIZING GRID END"));
}

void AGrid::ClearGrid() {
	UE_LOG(LogTemp, Warning, TEXT("CLEARING GRID START"));
	for (int32 i = 0; i < GridRows.Num(); i++) {
		for (int32 j = 0; j < GridRows[i].Columns.Num(); j++) {
			GridRows[i].Columns[j]->Destroy();
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("CLEARING GRID END"));
}

ATile* AGrid::PlaceWallTile(FVector Location)
{
	ATile* tileSpawned = nullptr;

	FActorSpawnParameters params;
	params.Owner = this;

	if (WallTile) {
		tileSpawned = GetWorld()->SpawnActor<ATile>(WallTile, Location, GetActorRotation(), params);
	}
	return tileSpawned;
}

ATile* AGrid::PlaceWalkTile(FVector Location)
{
	ATile* tileSpawned = nullptr;

	FActorSpawnParameters params;
	params.Owner = this;

	if (WalkTile) {
		tileSpawned = GetWorld()->SpawnActor<ATile>(WalkTile, Location, GetActorRotation(), params);
	}
	return tileSpawned;
}

ATile* AGrid::PlaceStartTile(FVector Location)
{
	ATile* tileSpawned = nullptr;

	FActorSpawnParameters params;
	params.Owner = this;

	if (StartTile) {
		tileSpawned = GetWorld()->SpawnActor<ATile>(StartTile, Location, GetActorRotation(), params);
	}
	return tileSpawned;
}

ATile* AGrid::PlaceGoalTile(FVector Location)
{
	ATile* tileSpawned = nullptr;

	FActorSpawnParameters params;
	params.Owner = this;

	if (GoalTile) {
		FVector TileLocation = GridRows[Location.X].Columns[Location.Y]->GetActorLocation();
		TileLocation.Z += 20;
		GridRows[Location.X].Columns[Location.Y]->Destroy();
		tileSpawned = GetWorld()->SpawnActor<ATile>(GoalTile, TileLocation, GetActorRotation(), params);
	}

	return tileSpawned;
}

FVector AGrid::GetRandomPointFarFrom(FVector Location)
{
	int32 GridSize = Size * 2 - 3;
	int32 addFirst = FMath::RandRange(0, 1);
	int32 addSecond;
	if (!addFirst) {
		addSecond = 1;
	}
	else {
		addSecond = FMath::RandRange(0, 1);
	}
	FVector selected = FVector(Location.X + GridSize * addFirst, Location.Y + GridSize * addSecond, 0);

	UE_LOG(LogTemp, Warning, TEXT("Selected: %s"), *selected.ToString());

	return selected;
}

void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AGrid::Hunt() {
	// Find an unvisited cell adjacent to a visited cell
	// Carve a passage between them
	// Restart the algorithm
	int32 GridSize = Size * 2 - 1;
	FVector ActualPosition;
	UE_LOG(LogTemp, Warning, TEXT("Let's hunt!"));
	for (int32 i = 1; i < GridSize; i += 2) {
		for (int32 j = 1; j < GridSize; j += 2) {
			ActualPosition = FVector(i, j, 0);
			if (!IsVisited(FVector(i, j, 0))) {
				UE_LOG(LogTemp, Warning, TEXT("Let's walk again"));
				TArray<FMovement> PossibleMovements = GetPossibleHuntMovementsFrom(ActualPosition);
				FMovement rndm = GetRandomMovementFrom(PossibleMovements);
				ApplyMovement(rndm);
				return ActualPosition;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("All hunted!"));
	return FVector(-1, 0, 0);
}

TArray<FMovement> AGrid::GetPossibleMovementsFrom(FVector Position)
{
	TArray<FMovement> PossibleMovements;
	FVector Movement;

	Movement = GoUpFrom(Position);
	if (IsValid(Movement) && !IsVisited(Movement)) {
		FMovement UpMovement;
		UpMovement.Movement = Movement;
		UpMovement.IntermediateTile = FVector(Movement.X,Movement.Y + 1,Movement.Z);
		UpMovement.MovementType = EMovementType::GOUP;
		PossibleMovements.Add(UpMovement);
	}
	Movement = GoDownFrom(Position);
	if (IsValid(Movement) && !IsVisited(Movement)) {
		FMovement DownMovement;
		DownMovement.Movement = Movement;
		DownMovement.IntermediateTile = FVector(Movement.X, Movement.Y - 1, Movement.Z);
		DownMovement.MovementType = EMovementType::GODOWN;
		PossibleMovements.Add(DownMovement);
	}
	Movement = GoRightFrom(Position);
	if (IsValid(Movement) && !IsVisited(Movement)) {
		FMovement RightMovement;
		RightMovement.Movement = Movement;
		RightMovement.IntermediateTile = FVector(Movement.X - 1, Movement.Y, Movement.Z);
		RightMovement.MovementType = EMovementType::GORIGHT;
		PossibleMovements.Add(RightMovement);
	}
	Movement = GoLeftFrom(Position);
	if (IsValid(Movement) && !IsVisited(Movement)) {
		FMovement LeftMovement;
		LeftMovement.Movement = Movement;
		LeftMovement.IntermediateTile = FVector(Movement.X + 1, Movement.Y, Movement.Z);
		LeftMovement.MovementType = EMovementType::GOLEFT;
		PossibleMovements.Add(LeftMovement);
	}

	return PossibleMovements;
}
TArray<FMovement> AGrid::GetPossibleHuntMovementsFrom(FVector Position)
{
	TArray<FMovement> PossibleMovements;
	FVector Movement;

	Movement = GoUpFrom(Position);
	if (IsValid(Movement) && IsVisited(Movement)) {
		FMovement UpMovement;
		UpMovement.Movement = Movement;
		UpMovement.IntermediateTile = FVector(Movement.X, Movement.Y + 1, Movement.Z);
		PossibleMovements.Add(UpMovement);
	}
	Movement = GoDownFrom(Position);
	if (IsValid(Movement) && IsVisited(Movement)) {
		FMovement DownMovement;
		DownMovement.Movement = Movement;
		DownMovement.IntermediateTile = FVector(Movement.X, Movement.Y - 1, Movement.Z);
		PossibleMovements.Add(DownMovement);
	}
	Movement = GoRightFrom(Position);
	if (IsValid(Movement) && IsVisited(Movement)) {
		FMovement RightMovement;
		RightMovement.Movement = Movement;
		RightMovement.IntermediateTile = FVector(Movement.X - 1, Movement.Y, Movement.Z);
		PossibleMovements.Add(RightMovement);
	}
	Movement = GoLeftFrom(Position);
	if (IsValid(Movement) && IsVisited(Movement)) {
		FMovement LeftMovement;
		LeftMovement.Movement = Movement;
		LeftMovement.IntermediateTile = FVector(Movement.X + 1, Movement.Y, Movement.Z);
		PossibleMovements.Add(LeftMovement);
	}

	return PossibleMovements;
}


bool AGrid::IsVisited(FVector Position) {
	return VisitedTiles.Contains(Position);
}

bool AGrid::IsValid(FVector Position) {
	bool isValid = (Position.X >= 1 && Position.Y >= 1) && (Position.X < (Size * 2 - 1) && Position.Y < (Size * 2 - 1));
	return isValid;
}

FMovement AGrid::GetRandomMovementFrom(TArray<FMovement> Movements)
{
	int32 Movement = FMath::RandRange(0, Movements.Num() - 1);
	return Movements[Movement];
}

FVector AGrid::GoUpFrom(FVector ActualPosition) {
	return FVector(ActualPosition.X, ActualPosition.Y - 2, ActualPosition.Z);
}
FVector AGrid::GoDownFrom(FVector ActualPosition) {
	return FVector(ActualPosition.X, ActualPosition.Y + 2, ActualPosition.Z);
}
FVector AGrid::GoRightFrom(FVector ActualPosition) {
	return FVector(ActualPosition.X + 2, ActualPosition.Y, ActualPosition.Z);
}
FVector AGrid::GoLeftFrom(FVector ActualPosition) {
	return FVector(ActualPosition.X - 2, ActualPosition.Y, ActualPosition.Z);
}

void AGrid::PostEditChangeProperty(FPropertyChangedEvent& e) {
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;

	if (PropertyName == FName("Size")) {
		UE_LOG(LogTemp, Warning, TEXT("Property Modified"));
		GenerateMaze();
	}
}


