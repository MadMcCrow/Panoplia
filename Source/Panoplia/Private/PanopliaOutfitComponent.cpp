/* Copyright © Noé Perard-Gayot 2021. Licensed under the MIT License. You may obtain a copy of the License at https://opensource.org/licenses/mit-license.php */


#include "PanopliaOutfitComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
 	

FName UPanopliaOutfitComponent::SpawnedComponentBaseName = "PanopliaMeshComponent";

UPanopliaOutfitComponent::UPanopliaOutfitComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UPanopliaOutfitComponent::BeginPlay()
{
	Super::BeginPlay();

	// Update Spawned components
	UpadeMeshComponents();
}


void UPanopliaOutfitComponent::AddMesh(TSoftObjectPtr<USkeletalMesh> Mesh)
{
	DesiredMeshes.AddUnique(Mesh);
	UpadeMeshComponents();
}


void UPanopliaOutfitComponent::UpadeMeshComponents()
{
	TArray<TSoftObjectPtr<USkeletalMesh>> CorrectMeshes;

	// first remove unecessary meshes and build array
	for (int idx = LayeredMeshComponents.Num(); idx --> 0; )
	{
		if (auto MeshComp = LayeredMeshComponents[idx])
		{
			auto SoftMesh = TSoftObjectPtr<USkeletalMesh>(MeshComp->SkeletalMesh);
			if (!DesiredMeshes.Contains(SoftMesh))
			{
				RemoveMeshComponent(MeshComp);
			}
			else
			{
				CorrectMeshes.AddUnique(SoftMesh);
			}

		}
	}

	// find missing meshes
	auto MissingMeshes = DesiredMeshes.FilterByPredicate([&CorrectMeshes](const TSoftObjectPtr<USkeletalMesh> &MeshItr) -> bool {return !CorrectMeshes.Contains(MeshItr);});

	// add the ones not present
	for ( TSoftObjectPtr<USkeletalMesh> MissingMesh : MissingMeshes)
	{
		if(MissingMesh.IsValid())
		{
			auto MeshComp = SpawnMeshComponent();
			MeshComp->SetSkeletalMesh(MissingMesh.Get());
		}
		else
		{
			AsyncMeshLoad(MissingMesh);
		}
	}
}


void UPanopliaOutfitComponent::AsyncMeshLoad(TSoftObjectPtr<USkeletalMesh> MeshTarget)
{
	if (MeshTarget.IsNull())
		return;
	
	FStreamableManager& Streamable =  UAssetManager::Get().GetStreamableManager();
	Streamable.RequestAsyncLoad(MeshTarget.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &UPanopliaOutfitComponent::OnMeshLoad));

}

void UPanopliaOutfitComponent::OnMeshLoad()
{
	UpadeMeshComponents();
}

USkeletalMeshComponent* UPanopliaOutfitComponent::SpawnMeshComponent()
{
	if (GetOwner())
	{
		const FName NewMeshName = MakeUniqueObjectName(GetOwner(),USkeletalMeshComponent::StaticClass(),SpawnedComponentBaseName);
		USkeletalMeshComponent* NewNeshComp = NewObject<USkeletalMeshComponent>(GetOwner(), NewMeshName);
		if(NewNeshComp)
		{
			NewNeshComp->RegisterComponent();
			NewNeshComp->AttachToComponent(GetMasterSkeletalMesh(), FAttachmentTransformRules::KeepWorldTransform, NAME_None);
			NewNeshComp->SetMasterPoseComponent(GetMasterSkeletalMesh());
			LayeredMeshComponents.Add(NewNeshComp);
			return NewNeshComp;
		}
	}
	return nullptr;
}

void UPanopliaOutfitComponent::RemoveMeshComponent(USkeletalMeshComponent* Mesh)
{
	LayeredMeshComponents.Remove(Mesh);
	Mesh->UnregisterComponent();
	Mesh->DestroyComponent();
}


USkeletalMeshComponent*  UPanopliaOutfitComponent::GetMasterSkeletalMesh() const
{
	if (auto CharOwner = Cast<ACharacter>(GetOwner()))
	{
		return CharOwner->GetMesh();
	}
	return nullptr;
}