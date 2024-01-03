#include "GaussianBlur.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"

void GaussianBlur::Initialize(uint32_t width, uint32_t height)
{
	textureWidth_ = width;
	textureHeight_ = height;

	//通常ぼかしのリソースを作成
	for (uint32_t i = 0; i < kCountOfBlurDirection; ++i)
	{
		blurBuffers_[i] = std::make_unique<ColorBuffer>();
		blurBuffers_[i]->Create(width, height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

		//ディスクリプターをコピー
		ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();
		descriptorHandles_[i] = TextureManager::GetInstance()->GetDescriptorHeap()->Allocate();
		device->CopyDescriptorsSimple(1, descriptorHandles_[i], blurBuffers_[i]->GetSRVHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	//高輝度ぼかしリソースを作成
	for (uint32_t i = 0; i < kCountOfBlurDirection; ++i)
	{
		highLumBlurBuffers_[i] = std::make_unique<ColorBuffer>();
		highLumBlurBuffers_[i]->Create(width, height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

		//ディスクリプターをコピー
		ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();
		highLumDescriptorHandles_[i] = TextureManager::GetInstance()->GetDescriptorHeap()->Allocate();
		device->CopyDescriptorsSimple(1, highLumDescriptorHandles_[i], highLumBlurBuffers_[i]->GetSRVHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	//ConstBufferの作成
	constBuff_ = std::make_unique<UploadBuffer>();
	constBuff_->Create(sizeof(ConstBuffDataGaussianBlur));
	Update();
}

void GaussianBlur::Update()
{
	ConstBuffDataGaussianBlur* gaussianBlurData = static_cast<ConstBuffDataGaussianBlur*>(constBuff_->Map());
	gaussianBlurData->textureWidth = textureWidth_;
	gaussianBlurData->textureHeight = textureHeight_;
	float total = 0.0f;
	for (int i = 0; i < 8; i++) {
		gaussianBlurData->weight[i] = expf(-(i * i) / (2 * sigma_ * sigma_));
		total += gaussianBlurData->weight[i];
	}
	total = total * 2.0f - 1.0f;
	//最終的な合計値で重みをわる
	for (int i = 0; i < 8; i++) {
		gaussianBlurData->weight[i] /= total;
	}
	constBuff_->Unmap();
}

void GaussianBlur::PreBlur(BlurDirection direction)
{
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();
	commandContext->TransitionResource(*blurBuffers_[direction], D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandContext->TransitionResource(*highLumBlurBuffers_[direction], D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	rtvHandles[0] = blurBuffers_[direction]->GetRTVHandle();
	rtvHandles[1] = highLumBlurBuffers_[direction]->GetRTVHandle();
	commandContext->SetRenderTargets(2, rtvHandles);
	commandContext->ClearColor(*blurBuffers_[direction]);
	commandContext->ClearColor(*highLumBlurBuffers_[direction]);

	//ビューポート
	D3D12_VIEWPORT viewport{};
	viewport.Width = FLOAT(textureWidth_);
	viewport.Height = FLOAT(textureHeight_);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	commandContext->SetViewport(viewport);

	//シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = textureWidth_;
	scissorRect.top = 0;
	scissorRect.bottom = textureHeight_;
	commandContext->SetScissor(scissorRect);

	//GaussianBlurDataの更新
	Update();
}

void GaussianBlur::PostBlur(BlurDirection direction)
{
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();
	commandContext->TransitionResource(*blurBuffers_[direction], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandContext->TransitionResource(*highLumBlurBuffers_[direction], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}