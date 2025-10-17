// ----------------------------------------------------------
// 頂点
// ----------------------------------------------------------
cbuffer VSConstants : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};

// 頂点シェーダーへ入力するデータ
struct VSInput
{
    float3 pos : POSITION;
    float4 color : COLOR0;
};


// 頂点シェーダーから出力するデータ
struct PSInput
{
    float4 pos : SV_POSITION; // 頂点の座標(射影座標系)
    float4 color : COLOR0;
};

// 頂点シェーダー
PSInput VS(VSInput vin)
{
    PSInput Out;
    float4 p = float4(vin.pos.xyz, 1);
    p = mul(world, p);
    p = mul(view, p);
    p = mul(projection, p);
    Out.pos = p;
    Out.color = vin.color;
    return Out;
}

 
// ----------------------------------------------------------
// ピクセル
// ----------------------------------------------------------

// ピクセルシェーダー
float4 PS(PSInput In) : SV_Target0
{
    // テクスチャの色を出力
    return In.color;
}
