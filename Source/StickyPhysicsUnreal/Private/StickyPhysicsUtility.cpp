// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyPhysicsUtility.h"
#include "CollisionQueryParams.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"

bool FStickyPhysicsUtility::TraceForTriangle(FStickyTriangle& Result, const UWorld* World, const FVector& TraceStart, const FVector& TraceEnd, ECollisionChannel Channel)
{
	if (!World) return false;
	
	static FCollisionQueryParams Params;
	Params.bReturnFaceIndex = true;
	Params.bTraceComplex = true;
	
	FHitResult HitResult;
	if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, Channel, Params))
	{
		const UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(HitResult.Component);
		if (StaticMeshComponent)
		{
			const UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh();

			if (!StaticMesh) 
				return false;
			if (!StaticMesh->RenderData) 
				return false;
			if (StaticMesh->RenderData->LODResources.Num() == 0) 
				return false;

			FStaticMeshLODResources* Resources =
				&(StaticMesh->RenderData->LODResources[StaticMesh->LODForCollision]);

			const int Index1 = Resources->IndexBuffer.GetIndex(HitResult.FaceIndex * 3);
			const int Index2 = Resources->IndexBuffer.GetIndex(HitResult.FaceIndex * 3 + 1);
			const int Index3 = Resources->IndexBuffer.GetIndex(HitResult.FaceIndex * 3 + 2);

			const FStaticMeshVertexBuffers* VertexBuffers = &(Resources->VertexBuffers);

			const FVector VertexPosition1 = VertexBuffers->PositionVertexBuffer.VertexPosition(Index1);
			const FVector VertexPosition2 = VertexBuffers->PositionVertexBuffer.VertexPosition(Index2);
			const FVector VertexPosition3 = VertexBuffers->PositionVertexBuffer.VertexPosition(Index3);

			Result = FStickyTriangle(VertexPosition1, VertexPosition2, VertexPosition3, StaticMeshComponent);
			return true;
		}
	}
	return false;
}
