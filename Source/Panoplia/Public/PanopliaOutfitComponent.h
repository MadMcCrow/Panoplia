/* Copyright © Noé Perard-Gayot 2021. Licensed under the MIT License. You may obtain a copy of the License at https://opensource.org/licenses/mit-license.php */

#pragma once

#include "Components/ActorComponent.h"
#include "PanopliaOutfitComponent.generated.h"

// forward declaration
class USkeletalMeshComponent;
class USkeletalMesh;


/**
 *	@class	UPanopliaOutfitComponent
 *	@brief	Component that spawns and update meshes dynamically
 *	@note	Restricted to ACharacter based classes for attachement/Anim issues
 *	@note	Will Load your Mesh in an ASync fashion !
 *	@todo	add merging
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PANOPLIA_API UPanopliaOutfitComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPanopliaOutfitComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:

	/**
	 *	@fn		AddMesh
	 *	@brief	Add a mesh to our layered meshes, will load it async
	 *	@note	if already present, wont do anything
	 *	@todo	add merging option
	 */
	UFUNCTION(BlueprintCallable, Category = "Panoplia")
	void AddMesh(TSoftObjectPtr<USkeletalMesh> Mesh);

protected:

	/**
	 *	DesiredMeshes
	 *	@brief	The meshes we want to have by default
	 *	@note	They are async loaded
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panoplia")
	TArray<TSoftObjectPtr<USkeletalMesh>> DesiredMeshes;


	UFUNCTION(BlueprintCallable, Category = "Panoplia")
	virtual void UpadeMeshComponents();


private:

	/**  request the streaming of a mesh */
	void AsyncMeshLoad(TSoftObjectPtr<USkeletalMesh> MeshTarget);

	/**  Called when a mesh is done streaming */
	void OnMeshLoad();

	/** Simply dynamically spawn a mesh component and add it to our owner */
	USkeletalMeshComponent* SpawnMeshComponent();

	/** Simply delete a spawned mesh component */
	void RemoveMeshComponent(USkeletalMeshComponent* Mesh);

	/**
	 *	LayeredMeshComponents
	 *	@brief	Spawned meshes layered on top of the main mesh
	 *	@note	They are dynamically added and removed on request
	 *	@todo	When adding merging, this must be cleaned
	 */
	UPROPERTY(Transient)
	TArray<USkeletalMeshComponent*> LayeredMeshComponents;

	USkeletalMeshComponent*  GetMasterSkeletalMesh() const;



	static FName SpawnedComponentBaseName;

		
	
};
