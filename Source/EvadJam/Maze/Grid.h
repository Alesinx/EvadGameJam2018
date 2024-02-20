// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"

USTRUCT()
struct FGridColumn {
	GENERATED_BODY()
	
public:
	TArray<class ATile*> Columns;
};

UENUM()
enum EMovementType {
	GOUP,
	GODOWN,
	GORIGHT,
	GOLEFT
};

USTRUCT()
struct FMovement {
	GENERATED_BODY()

public:
	EMovementType MovementType;
	FVector Movement;
	FVector IntermediateTile;
};



UCLASS()
class EVADJAM_API AGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrid();

private:
	TArray<FVector> VisitedTiles;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void InitializeGrid();
	void ClearGrid();
	UFUNCTION(BlueprintCallable)
	void GenerateMaze();
	void WalkFrom(FVector StartPosition);
	FVector Hunt();
	TArray<FMovement> GetPossibleMovementsFrom(FVector ActualPosition);
	TArray<FMovement> GetPossibleHuntMovementsFrom(FVector Position);
	FMovement GetRandomMovementFrom(TArray<FMovement> Movements);
	FVector GoUpFrom(FVector ActualPosition);
	FVector GoDownFrom(FVector ActualPosition);
	FVector GoRightFrom(FVector ActualPosition);
	FVector GoLeftFrom(FVector ActualPosition);
	void ApplyMovement(FMovement Movement);
	bool IsValid(FVector Position);
	bool IsVisited(FVector Position);
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
	TArray<FGridColumn> GridRows;
	ATile* PlaceWallTile(FVector Location);
	ATile* PlaceWalkTile(FVector Location);
	ATile * PlaceStartTile(FVector Location);
	ATile * PlaceGoalTile(FVector Location);
	FVector GetRandomPointFarFrom(FVector Location);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Grid")
	int32 Size;
	UPROPERTY(EditAnywhere, Category = "Grid")
	TSubclassOf<class ATile> WallTile;
	UPROPERTY(EditAnywhere, Category = "Grid")
	TSubclassOf<class ATile> WalkTile;
	UPROPERTY(EditAnywhere, Category = "Grid")
	TSubclassOf<class ATile> StartTile;
	UPROPERTY(EditAnywhere, Category = "Grid")
	TSubclassOf<class ATile> GoalTile;
	int32 TileSize;
	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	ATile* GoalTileActor;
	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	ATile* StartTileActor;
};
