// CreateDefaultScene.cpp
// デフォルトのシーンを生成します

#include <numbers>

#include <UniDx.h>
#include <UniDx/Scene.h>
#include <UniDx/Camera.h>
#include <UniDx/PrimitiveRenderer.h>
#include <UniDx/GltfModel.h>
#include <UniDx/Rigidbody.h>
#include <UniDx/Random.h>
#include <UniDx/Collider.h>
#include <UniDx/Light.h>
#include <UniDx/Canvas.h>
#include <UniDx/TextMesh.h>
#include <UniDx/Font.h>
#include <UniDx/Image.h>

#include "CameraBehaviour.h"
#include "Hinge.h"
#include "ChainRender.h"

using namespace std;
using namespace UniDx;

// VertexPNTにウェイトWを追加した頂点情報
struct VertexPNTW
{
    Vector3 position;
    Vector3 normal;
    Vector2 uv0;
    float   weight;

    void setPosition(Vector3 v) { position = v; }
    void setNormal(Vector3 v) { normal = v; }
    void setColor(Color c) {}
    void setUV(Vector2 v) { uv0 = v; }
    void setUV2(Vector2 v) {}
    void setUV3(Vector2 v) {}
    void setUV4(Vector2 v) {}
    void setWeight(float w) { weight = w; }

    static const std::array< D3D11_INPUT_ELEMENT_DESC, 4> layout;
};
const std::array< D3D11_INPUT_ELEMENT_DESC, 4> VertexPNTW::layout =
{
    D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    D3D11_INPUT_ELEMENT_DESC{ "TEXUV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    D3D11_INPUT_ELEMENT_DESC{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


unique_ptr<Scene> CreateDefaultScene()
{
    // -- チェーン --
    auto chains =
        make_unique<GameObject>(L"チェーン", Vector3(-1, 3, 0),
            ChainRender::create<VertexPNT>(2, L"Resource/AlbedoShade.hlsl", L"Resource/wood-1.png"),

            make_unique<GameObject>(L"ジョイント", Vector3(2, 0, 0),

                make_unique<GameObject>(L"子チェーン",
                    make_unique<Hinge>(),
                    ChainRender::create<VertexPNT>(4, L"Resource/AlbedoShade.hlsl", L"Resource/wood-1.png")
                )
            )
        );
    chains->transform->localScale = Vector3(0.5, 0.5, 0.5);

    auto skinChains =
        make_unique<GameObject>(L"チェーン", Vector3(-1, 1.5, 0),
            ChainRender::create<VertexPNT>(2, L"Resource/AlbedoShade.hlsl", L"Resource/wood-1.png"),

            make_unique<GameObject>(L"ジョイント", Vector3(2, 0, 0),

                make_unique<GameObject>(L"子チェーン",
                    make_unique<Hinge>(),
                    ChainRender::create<VertexPNT>(4, L"Resource/AlbedoShade.hlsl", L"Resource/wood-1.png")
                )
            )
        );
    skinChains->transform->localScale = Vector3(0.5, 0.5, 0.5);

    // -- 床 --
    // キューブレンダラを作ってサイズを調整
    auto rb = make_unique<Rigidbody>();
    rb->gravityScale = 0;
    rb->mass = numeric_limits<float>::infinity();
    auto floor = make_unique<GameObject>(L"床",
        CubeRenderer::create<VertexPNT>(L"Resource/AlbedoShade.hlsl", L"Resource/brick-1.png"),
        move(rb),
        make_unique<AABBCollider>());
    floor->transform->localPosition = Vector3(0.0f, -0.5f, 0.0f);
    floor->transform->localScale = Vector3(5, 1, 5);

    // -- カメラ --
    auto cameraBehaviour = make_unique<CameraBehaviour>();

    // -- ライト --
    auto light = make_unique<GameObject>(L"ライト",
        make_unique<Light>());
    light->transform->localRotation = Quaternion::CreateFromYawPitchRoll(0.2f, 0.9f, 0);

    // -- UI --
    auto font = make_shared<Font>();
    font->Load(L"Resource/M PLUS 1.spritefont");
    auto textMesh = make_unique<TextMesh>();
    textMesh->font = font;
    textMesh->text = L"-";
    auto textObj = make_unique<GameObject>(L"テキスト", textMesh);
    textObj->transform->localPosition = Vector3(100, 20, 0);

    auto canvas = make_unique<Canvas>();
    canvas->LoadDefaultMaterial(L"Resource");

    // シーンを作って戻す
    return make_unique<Scene>(

        make_unique<GameObject>(L"オブジェクトルート",
            move(chains),
            move(skinChains),
            move(floor)
        ),

        move(light),

        make_unique<GameObject>(L"カメラルート", Vector3(0, 3, -5),
            make_unique<Camera>(),
            move(cameraBehaviour)
        ),

        make_unique<GameObject>(L"UI",
            move(canvas),
            move(textObj)
        )
    );
}
