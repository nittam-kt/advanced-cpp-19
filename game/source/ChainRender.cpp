#include "ChainRender.h"

#include <UniDx/D3DManager.h>
#include <UniDx/Camera.h>

using namespace UniDx;

// -----------------------------------------------------------------------------
// 頂点シェーダー側と共有する、ローカル行列の定数バッファ
// -----------------------------------------------------------------------------
struct LocalConstants
{
    Matrix local;
};


void ChainRender::OnEnable()
{
    MeshRenderer::OnEnable();

    // メッシュの初期化
    auto submesh = std::make_unique<SubMesh>();
    submesh->topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // 頂点情報の作成
    createVertex();

    submesh->positions = std::span<const Vector3>(positions.data(), positions.size());
    submesh->uv = std::span<const Vector2>(uv.data(), uv.size());
    submesh->normals = std::span<const Vector3>(normals.data(), normals.size());
    submesh->indices = std::span<const uint32_t>(indices.data(), indices.size());

    if (createBufer_ != nullptr)
    {
        createBufer_(this, submesh.get());
    }

    mesh.submesh.push_back(std::move(submesh));

    // CBuffer1 = ローカル行列バッファ の作成
    createCBuffer1();
}


void ChainRender::updatePositionCameraCBuffer(const UniDx::Camera& camera) const
{
    if (!enableSkin)
    {
        // ブレンドが有効でない場合はベースと同じ
        MeshRenderer::updatePositionCameraCBuffer(camera);
        return;
    }

    // constantBuffer0 には親のワールド行列を指定
    VSConstantBuffer0 cb0{};
    if (transform->parent != nullptr)
    {
        cb0.world = transform->parent->getLocalToWorldMatrix();
    }
    else
    {
        cb0.world = Matrix::Identity;
    }
    cb0.view = camera.GetViewMatrix();
    cb0.projection = camera.GetProjectionMatrix(16.0f / 9.0f);

    // 定数バッファ更新
    ID3D11Buffer* cbs0[1] = { constantBuffer0.Get() };
    D3DManager::getInstance()->GetContext()->VSSetConstantBuffers(0, 1, cbs0);
    D3DManager::getInstance()->GetContext()->UpdateSubresource(constantBuffer0.Get(), 0, nullptr, &cb0, 0, 0);

    // constantBuffer1 に自身のローカル行列を指定して定数バッファを更新
    ID3D11Buffer* cbs1[1] = { constantBuffer1.Get() };
    D3DManager::getInstance()->GetContext()->VSSetConstantBuffers(1, 1, cbs1);
    LocalConstants cb1{ transform->GetLocalMatrix() };
    D3DManager::getInstance()->GetContext()->UpdateSubresource(constantBuffer1.Get(), 0, nullptr, &cb1, 0, 0);
}


void ChainRender::createVertex()
{
    // 頂点・UV・法線・インデックス初期化
    positions.clear();
    uv.clear();
    normals.clear();
    indices.clear();
    weights.clear();

    // キューブの6面のうち、X方向の隣接面は省略
    // 残す面: +Y, -Y, +Z, -Z, +X(端), -X(端)
    // 各面ごとに4頂点・2三角形

    for (int i = 0; i < chainNum; ++i) {
        float x = static_cast<float>(i);

        // 端の面のみ
        if (i == 0) {
            // -X面
            Vector3 normal(-1, 0, 0);
            std::array<Vector3, 4> verts = {
                Vector3(x, -0.5, 0),
                Vector3(x, 0.5, 0),
                Vector3(x, 0.5, 1),
                Vector3(x, -0.5, 1)
            };
            for (int v = 0; v < 4; ++v) {
                positions.push_back(verts[v]);
                uv.push_back(Vector2(0, float(v % 2))); // u=0固定, vループ
                normals.push_back(normal);
                weights.push_back(0);
            }
            uint32_t base = static_cast<uint32_t>(positions.size()) - 4;
            indices.insert(indices.end(), { base, base + 2, base + 1, base, base + 3, base + 2 });
        }
        if (i == chainNum - 1) {
            // +X面
            Vector3 normal(1, 0, 0);
            std::array<Vector3, 4> verts = {
                Vector3(x + 1, -0.5, 0),
                Vector3(x + 1, -0.5, 1),
                Vector3(x + 1, 0.5, 1),
                Vector3(x + 1, 0.5, 0)
            };
            for (int v = 0; v < 4; ++v) {
                positions.push_back(verts[v]);
                uv.push_back(Vector2(1, float(v % 2))); // u=1固定, vループ
                normals.push_back(normal);
                weights.push_back(1);
            }
            uint32_t base = static_cast<uint32_t>(positions.size()) - 4;
            indices.insert(indices.end(), { base, base + 2, base + 1, base, base + 3, base + 2 });
        }

        // +Y面
        {
            Vector3 normal(0, 1, 0);
            std::array<Vector3, 4> verts = {
                Vector3(x, 0.5, 0),
                Vector3(x + 1, 0.5, 0),
                Vector3(x + 1, 0.5, 1),
                Vector3(x, 0.5, 1)
            };
            for (int v = 0; v < 4; ++v) {
                float u = (float)i / chainNum;
                float u2 = (float)(i + 1) / chainNum;
                float vval = (v < 2) ? 0.0f : 1.0f;
                positions.push_back(verts[v]);
                uv.push_back(Vector2(v == 0 || v == 3 ? u : u2, vval));
                normals.push_back(normal);
                weights.push_back(v == 0 || v == 3 ? u : u2);
            }
            uint32_t base = static_cast<uint32_t>(positions.size()) - 4;
            indices.insert(indices.end(), { base, base + 2, base + 1, base, base + 3, base + 2 });
        }
        // -Y面
        {
            Vector3 normal(0, -1, 0);
            std::array<Vector3, 4> verts = {
                Vector3(x, -0.5, 0),
                Vector3(x, -0.5, 1),
                Vector3(x + 1, -0.5, 1),
                Vector3(x + 1, -0.5, 0)
            };
            for (int v = 0; v < 4; ++v) {
                float u = (float)i / chainNum;
                float u2 = (float)(i + 1) / chainNum;
                float vval = (v < 2) ? 0.0f : 1.0f;
                positions.push_back(verts[v]);
                uv.push_back(Vector2(v < 2 ? u : u2, vval));
                normals.push_back(normal);
                weights.push_back(v < 2 ? u : u2);
            }
            uint32_t base = static_cast<uint32_t>(positions.size()) - 4;
            indices.insert(indices.end(), { base, base + 2, base + 1, base, base + 3, base + 2 });
        }
        // +Z面
        {
            Vector3 normal(0, 0, 1);
            std::array<Vector3, 4> verts = {
                Vector3(x, -0.5, 1),
                Vector3(x, 0.5, 1),
                Vector3(x + 1, 0.5, 1),
                Vector3(x + 1, -0.5, 1)
            };
            for (int v = 0; v < 4; ++v) {
                float u = (float)i / chainNum;
                float u2 = (float)(i + 1) / chainNum;
                float vval = (v < 2) ? 0.0f : 1.0f;
                positions.push_back(verts[v]);
                uv.push_back(Vector2(v < 2 ? u : u2, vval));
                normals.push_back(normal);
                weights.push_back(v < 2 ? u : u2);
            }
            uint32_t base = static_cast<uint32_t>(positions.size()) - 4;
            indices.insert(indices.end(), { base, base + 2, base + 1, base, base + 3, base + 2 });
        }
        // -Z面
        {
            Vector3 normal(0, 0, -1);
            std::array<Vector3, 4> verts = {
                Vector3(x, -0.5, 0),
                Vector3(x + 1, -0.5, 0),
                Vector3(x + 1, 0.5, 0),
                Vector3(x, 0.5, 0)
            };
            for (int v = 0; v < 4; ++v) {
                float u = (float)i / chainNum;
                float u2 = (float)(i + 1) / chainNum;
                float vval = (v < 2) ? 0.0f : 1.0f;
                positions.push_back(verts[v]);
                uv.push_back(Vector2(v == 0 || v == 3 ? u : u2, vval));
                normals.push_back(normal);
                weights.push_back(v == 0 || v == 3 ? u : u2);
            }
            uint32_t base = static_cast<uint32_t>(positions.size()) - 4;
            indices.insert(indices.end(), { base, base + 2, base + 1, base, base + 3, base + 2 });
        }
    }
}


void ChainRender::createCBuffer1()
{
    // 行列用の定数バッファ生成
    D3D11_BUFFER_DESC desc{};
    desc.ByteWidth = sizeof(LocalConstants);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    D3DManager::getInstance()->GetDevice()->CreateBuffer(&desc, nullptr, constantBuffer1.GetAddressOf());
}
