struct Well
{
    float32_t4x4 skeletonSpaceMatrix;
    float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};

struct Vertex
{
    float32_t4 position;
    float32_t2 texcoord;
    float32_t3 normal;
};

struct VertexInfluence
{
    float32_t4 weight;
    int32_t4 index;
};

struct SkinningInformation
{
    uint32_t numVertices;
};

StructuredBuffer<Well> gMatrixPalette : register(t0);
StructuredBuffer<Vertex> gInputVertices : register(t1);
StructuredBuffer<VertexInfluence> gInfluences : register(t2);
RWStructuredBuffer<Vertex> gOutputVertices : register(u0);
ConstantBuffer<SkinningInformation> gSkinningInformation : register(b0);

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID)
{
    uint32_t vertexIndex = DTid.x;
    if (vertexIndex < gSkinningInformation.numVertices)
    {
        //必要なデータをStructuredBufferから取ってくる
        Vertex input = gInputVertices[vertexIndex];
        VertexInfluence influence = gInfluences[vertexIndex];
        
        //Skinning後の頂点を計算
        Vertex skinned;
        skinned.texcoord = input.texcoord;
        //位置の変換
        skinned.position = mul(input.position, gMatrixPalette[gInfluences[vertexIndex].index.x].skeletonSpaceMatrix) * gInfluences[vertexIndex].weight.x;
        skinned.position += mul(input.position, gMatrixPalette[gInfluences[vertexIndex].index.y].skeletonSpaceMatrix) * gInfluences[vertexIndex].weight.y;
        skinned.position += mul(input.position, gMatrixPalette[gInfluences[vertexIndex].index.z].skeletonSpaceMatrix) * gInfluences[vertexIndex].weight.z;
        skinned.position += mul(input.position, gMatrixPalette[gInfluences[vertexIndex].index.w].skeletonSpaceMatrix) * gInfluences[vertexIndex].weight.w;
        skinned.position.w = 1.0f; //確実に1を入れる
    
        //法線の変換
        skinned.normal = mul(input.normal, (float32_t3x3) gMatrixPalette[gInfluences[vertexIndex].index.x].skeletonSpaceInverseTransposeMatrix) * gInfluences[vertexIndex].weight.x;
        skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[gInfluences[vertexIndex].index.y].skeletonSpaceInverseTransposeMatrix) * gInfluences[vertexIndex].weight.y;
        skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[gInfluences[vertexIndex].index.z].skeletonSpaceInverseTransposeMatrix) * gInfluences[vertexIndex].weight.z;
        skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[gInfluences[vertexIndex].index.w].skeletonSpaceInverseTransposeMatrix) * gInfluences[vertexIndex].weight.w;
        skinned.normal = normalize(skinned.normal);
        
        //Skinning後の頂点データを格納
        gOutputVertices[vertexIndex] = skinned;
    }
}