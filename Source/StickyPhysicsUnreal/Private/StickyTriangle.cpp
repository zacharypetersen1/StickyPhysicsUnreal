// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyTriangle.h"
#include "DrawDebugHelpers.h"
#include "Components/SceneComponent.h"

FStickyTriangle::FStickyTriangle()
{
}

FStickyTriangle::FStickyTriangle(FVector VertexPositionA, FVector VertexPositionB, FVector VertexPositionC, const USceneComponent* SetOwningComponent)
{
	ObjectVertexPositions.Add(VertexPositionA);
	ObjectVertexPositions.Add(VertexPositionB);
	ObjectVertexPositions.Add(VertexPositionC);
	OwningComponent = SetOwningComponent;
}

FVector FStickyTriangle::GetVertexWorldPosition(int Index)
{
	if (IsValid(OwningComponent))
	{
		return OwningComponent->GetComponentTransform().TransformPosition(ObjectVertexPositions[Index]);
	}
}

FVector FStickyTriangle::GetVertexWorldNormal(int Index)
{
	return OwningComponent->GetComponentTransform().TransformVectorNoScale(ObjectVertexNormals[Index]);
}

FVector FStickyTriangle::GetWorldFaceNormal()
{
	return OwningComponent->GetComponentTransform().TransformVectorNoScale(ObjectFaceNormal);
}

FVector FStickyTriangle::GetWorldInterpolatedNormal(const FVector &AtThisLocation)
{
	FVector BaryCentricCoords = FMath::ComputeBaryCentric2D(
		AtThisLocation,
		GetVertexWorldPosition(0),
		GetVertexWorldPosition(1),
		GetVertexWorldPosition(2)
	);
	return GetVertexWorldNormal(0) * BaryCentricCoords[0] +
		GetVertexWorldNormal(1) * BaryCentricCoords[0] +
		GetVertexWorldNormal(2) * BaryCentricCoords[0];
}

FVector FStickyTriangle::ProjectDirectionOntoTriangle(const FVector &Direction)
{
	return FVector::VectorPlaneProject(Direction, ObjectFaceNormal);
}

FVector FStickyTriangle::ProjectLocationOntoTriangle(const FVector &Location)
{
	return FVector::VectorPlaneProject(Location - ObjectVertexPositions[0], ObjectFaceNormal) + ObjectVertexPositions[0];
}

bool FStickyTriangle::ProjectRayOntoBoundaries(FVector &ResultLocation, const FRay &Ray)
{
	float ShortestDistSquared = INFINITY;
	bool bFoundValidIntersection = false;

	// Construct plane that lies on each edge of triangle and is perpendicular to triangle and intersect ray with it
	for (int i = 0; i < 3; i++)
	{
		FVector PlaneOrigin = GetVertexWorldPosition(i);
		FVector PlaneNormal = ((GetVertexWorldPosition((i + 1) % 3) - PlaneOrigin) ^ GetWorldFaceNormal()).GetSafeNormal();

		// Check if Ray is orthogonal to line
		if (!FVector::Orthogonal(PlaneNormal, Ray.Direction, 0.00001f))
		{
			FPlane Plane(PlaneOrigin, PlaneNormal);
			FVector IntersectionPoint = FMath::RayPlaneIntersection(Ray.Origin, Ray.Direction, Plane);
			float DistSquared = FVector::DistSquared(IntersectionPoint, Ray.Origin);
			if (DistSquared < ShortestDistSquared)
			{
				bFoundValidIntersection = true;
				ShortestDistSquared = DistSquared;
				ResultLocation = IntersectionPoint;
			}
		}
	}
	return bFoundValidIntersection;
}

void FStickyTriangle::DebugDraw(const UWorld* World, const FColor &Color)
{
	for (int i = 0; i < 3; i++)
	{
		FTransform OwnerTransform = OwningComponent->GetComponentTransform();
		DrawDebugLine(World, 
			GetVertexWorldPosition(i), 
			GetVertexWorldPosition((i + 1) % 3),
			Color, false, -1.0f, 0, 5.0f);
	}
}
