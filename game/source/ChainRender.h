#pragma once

#include <functional> 
#include <UniDx.h>
#include <UniDx/Renderer.h>

// サイズ１のキューブをN個繋げるメッシュ
class ChainRender : public UniDx::MeshRenderer
{
public:
    explicit ChainRender(int num) : chainNum(num) {};

    template<typename TVertex>
    static std::unique_ptr<ChainRender> create(int num, const std::wstring& shaderPath)
    {
        auto ptr = std::unique_ptr<ChainRender>(new ChainRender(num));
        ptr->AddMaterial<TVertex>(shaderPath);
        ptr->createBufer_ = [](ChainRender* r, UniDx::SubMesh* submesh) { submesh->createBuffer<TVertex>(); };
        ptr->enableSkin = false;
        return ptr;
    }
    template<typename TVertex>
    static std::unique_ptr<ChainRender> createWithSkin(int num, const std::wstring& shaderPath)
    {
        auto ptr = std::unique_ptr<ChainRender>(new ChainRender(num));
        ptr->AddMaterial<TVertex>(shaderPath);
        ptr->createBufer_ = [](ChainRender* r, UniDx::SubMesh* submesh) {
            auto buf = submesh->createBuffer<TVertex>();
            for (int i = 0; i < r->weights.size(); ++i) (*buf)[i].weight = r->weights[i];
        };
        ptr->enableSkin = true;
        return ptr;
    }
    template<typename TVertex>
    static std::unique_ptr<ChainRender> create(int num, const std::wstring& shaderPath, const std::wstring& texturePath)
    {
        auto ptr = std::unique_ptr<ChainRender>(new ChainRender(num));
        ptr->AddMaterial<TVertex>(shaderPath, texturePath);
        ptr->createBufer_ = [](ChainRender* r, UniDx::SubMesh* submesh) { submesh->createBuffer<TVertex>(); };
        ptr->enableSkin = false;
        return ptr;
    }
    template<typename TVertex>
    void copyWeight(std::span<TVertex> buf)
    {
        for (int i = 0; i < weights.size(); ++i)
        {
            buf[i].weight = weights[i];
        }
    }

    template<typename TVertex>
    static std::unique_ptr<ChainRender> createWithSkin(int num, const std::wstring& shaderPath, const std::wstring& texturePath)
    {
        auto ptr = std::unique_ptr<ChainRender>(new ChainRender(num));
        ptr->AddMaterial<TVertex>(shaderPath, texturePath);
        ptr->createBufer_ = [](ChainRender* r, UniDx::SubMesh* submesh) {
            submesh->createBuffer<TVertex>(
                [r](std::span<TVertex> buf) { r->copyWeight(buf); }
            );
        };
        ptr->enableSkin = true;
        return ptr;
    }

protected:
    virtual void OnEnable() override;
    virtual void updatePositionCameraCBuffer(const UniDx::Camera& camera) const override;

    ComPtr<ID3D11Buffer> constantBuffer1;   // スキン用ローカル行列バッファ
    std::function<void(ChainRender* renderer, UniDx::SubMesh*)> createBufer_;
    std::vector<Vector3> positions;
    std::vector<Vector2> uv;
    std::vector<Vector3> normals;
    std::vector<uint32_t> indices;
    std::vector<float> weights;
    int chainNum;
    bool enableSkin = false;

private:
    void createVertex();
    void createCBuffer1();
};

