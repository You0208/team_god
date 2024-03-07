#include "Framebuffer.h"
#include "misc.h"

Framebuffer::Framebuffer(ID3D11Device* device, uint32_t width, uint32_t height, bool has_depthstencil /*BLOOM*/)
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // DXGI_FORMAT_R8G8B8A8_UNORM
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2d_desc, 0, render_target_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc{};
	render_target_view_desc.Format = texture2d_desc.Format;
	render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = device->CreateRenderTargetView(render_target_buffer.Get(), &render_target_view_desc, render_target_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
	shader_resource_view_desc.Format = texture2d_desc.Format;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(render_target_buffer.Get(), &shader_resource_view_desc, shader_resource_views[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// BLOOM
	if (has_depthstencil)
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
		texture2d_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		hr = device->CreateTexture2D(&texture2d_desc, 0, depth_stencil_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Flags = 0;
		hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, depth_stencil_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		shader_resource_view_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &shader_resource_view_desc, shader_resource_views[1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
}

void Framebuffer::Clear(ID3D11DeviceContext* immediate_context, float r, float g, float b, float a, float depth)
{
	float color[4]{ r, g, b, a };
	// �����_�[�^�[�Q�b�g���̂��ׂĂ̗v�f����̒l�ɐݒ�
	immediate_context->ClearRenderTargetView(
		render_target_view.Get(),	//�@�����_�[�^�[�Q�b�g�ւ̃|�C���^
		color						// �����_�[�^�[�Q�b�g��h��Ԃ��F��\���S�v�f�̔z��
	);
	if (depth_stencil_view) // BLOOM
	{
		// �[�x�X�e���V�����\�[�X���N���A
		immediate_context->ClearDepthStencilView(
			depth_stencil_view.Get(),	// �N���A����[�x�X�e���V���ւ̃|�C���^�[
			D3D11_CLEAR_DEPTH,			// ��������f�[�^�̎�ނ����
			depth,						// ���̒l���g�p���Đ[�x�o�b�t�@�[���N���A ���̒l�� 0 ���� 1 �̊ԂŃN�����v
			0							// ���̒l���g�p���ăX�e���V�� �o�b�t�@�[���N���A
		);
	}
}

void Framebuffer::Activate(ID3D11DeviceContext* immediate_context)
{
	viewport_count = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

	// ���X�^���C�U�X�e�[�W�Ɍ��ݓn����Ă���r���[�|�[�g�̐���ϐ��ɑ��
	immediate_context->RSGetViewports(
		&viewport_count,	// �r���[�|�[�g�̐����w�肷��ϐ��ւ̃|�C���^�[
		cached_viewports	// ���X�^���C�U�X�e�[�W�Ƀo�C���h����Ă���r���[�|�[�g�̍\���̂̔z��
	);

	// �o�̓}�[�W���[�X�e�[�W�ɓn���ꂽ���\�[�X�ւ̃|�C���^�[���擾
	// �i��ʂɏ��������̂��ǂ��ɂ��邩��������菇�j
	immediate_context->OMGetRenderTargets(
		1,													// �擾���郌���_�[�^�[�Q�b�g�̐�
		cached_render_target_view.ReleaseAndGetAddressOf(),	// �����_�[�^�[�Q�b�g�r���[��\���z��ւ̃|�C���^�[
		cached_depth_stencil_view.ReleaseAndGetAddressOf()	// �[�x�X�e���V���r���[�ւ̃|�C���^�[
	);

	// �r���[�|�[�g�̔z����p�C�v���C���̃��X�^���C�U�[�X�e�[�W�ɓn��
	// �i�����ɕ`������Ăق��������w���j
	immediate_context->RSSetViewports(
		1,					// �o�C���h����r���[�|�[�g�̐�
		&viewport			// �r���[�|�[�g�\���̂̔z��
	);

	// �S�Ẵo�b�t�@�̐ݒ肪�����ɍs���o�̓}�[�W���[�X�e�[�W�ɐ[�x�X�e���V���o�b�t�@��n��
	immediate_context->OMSetRenderTargets(
		1,									// �o�C���h���郌���_�[ �^�[�Q�b�g�̐�
		render_target_view.GetAddressOf(),	// �����_�[�^�[�Q�b�g�̔z��ւ̃|�C���^�[
		depth_stencil_view.Get()			// �[�x�X�e���V�� �r���[�̔z��ւ̃|�C���^�[
	);
}

void Framebuffer::Deactivate(ID3D11DeviceContext* immediate_context)
{
	// �r���[�|�[�g�̔z����p�C�v���C���̃��X�^���C�U�[�X�e�[�W�ɓn��
	// �i�����ɕ`������Ăق��������w���j
	immediate_context->RSSetViewports(
		viewport_count,				// �o�C���h����r���[�|�[�g�̐�
		cached_viewports			// �r���[�|�[�g�\���̂̔z��
	);

	// �S�Ẵo�b�t�@�̐ݒ肪�����ɍs���o�̓}�[�W���[�X�e�[�W�ɐ[�x�X�e���V���o�b�t�@��n��
	immediate_context->OMSetRenderTargets(
		1,											// �o�C���h���郌���_�[ �^�[�Q�b�g�̐�
		cached_render_target_view.GetAddressOf(),	// �����_�[�^�[�Q�b�g�̔z��ւ̃|�C���^�[
		cached_depth_stencil_view.Get()				// �[�x�X�e���V�� �r���[�̔z��ւ̃|�C���^�[
	);
}
