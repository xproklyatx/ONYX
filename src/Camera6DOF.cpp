#include "Camera6DOF.hpp"
#include "DeltaTime.hpp"
#include "Input.hpp"
#include <cmath>

Camera6DOF::Camera6DOF()
{
    position = {0.0f, 0.0f, -5.0f};
    pitch = 0.0f;
    yaw = 0.0f;
    fov = 45.0f;
    bFocused = false;
    UpdateVectors();
}

void Camera6DOF::Update()
{

    if (Input::IsKeyPressed(VK_ESCAPE))
    {
        ToggleFocus();
        return;
    }

    if (!bFocused && (Input::IsKeyPressed(VK_LBUTTON) || Input::IsKeyPressed(VK_RBUTTON)))
    {
        SetFocus(true);
        return;
    }

    ProcessInput();
    UpdateVectors();
}

void Camera6DOF::SetFocus(bool focused)
{
    bFocused = focused;
    HWND hWnd = GetActiveWindow();

    if (bFocused)
    {
        ShowCursor(FALSE);

        RECT rect;
        GetClientRect(hWnd, &rect);
        centerPos.x = (rect.left + rect.right) / 2;
        centerPos.y = (rect.top + rect.bottom) / 2;

        POINT screenCenter = centerPos;
        ClientToScreen(hWnd, &screenCenter);
        SetCursorPos(screenCenter.x, screenCenter.y);
        lastMousePos = centerPos;

        POINT topLeft = {rect.left, rect.top};
        POINT bottomRight = {rect.right, rect.bottom};
        ClientToScreen(hWnd, &topLeft);
        ClientToScreen(hWnd, &bottomRight);

        RECT screenRect;
        screenRect.left = topLeft.x;
        screenRect.top = topLeft.y;
        screenRect.right = bottomRight.x;
        screenRect.bottom = bottomRight.y;
        ClipCursor(&screenRect);
    }
    else
    {
        ShowCursor(TRUE);
        ClipCursor(nullptr);
    }
}

void Camera6DOF::ToggleFocus()
{
    SetFocus(!bFocused);
}

void Camera6DOF::ProcessInput()
{
    float dt = DeltaTime::GetDelta();

    if (!bFocused)
        return;

    if (Input::IsKeyDown('W') || Input::IsKeyDown('w'))
    {
        DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR fwd = DirectX::XMLoadFloat3(&forward);
        pos += fwd * moveSpeed * dt;
        DirectX::XMStoreFloat3(&position, pos);
        bViewDirty = true;
    }
    if (Input::IsKeyDown('S') || Input::IsKeyDown('s'))
    {
        DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR fwd = DirectX::XMLoadFloat3(&forward);
        pos -= fwd * moveSpeed * dt;
        DirectX::XMStoreFloat3(&position, pos);
        bViewDirty = true;
    }

    if (Input::IsKeyDown('A') || Input::IsKeyDown('a'))
    {
        DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&this->right);
        pos -= right * moveSpeed * dt;
        DirectX::XMStoreFloat3(&position, pos);
        bViewDirty = true;
    }
    if (Input::IsKeyDown('D') || Input::IsKeyDown('d'))
    {
        DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&this->right);
        pos += right * moveSpeed * dt;
        DirectX::XMStoreFloat3(&position, pos);
        bViewDirty = true;
    }

    if (Input::IsKeyDown(VK_SPACE))
    {
        DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&this->up);
        pos += up * moveSpeed * dt;
        DirectX::XMStoreFloat3(&position, pos);
        bViewDirty = true;
    }
    if (Input::IsKeyDown(VK_SHIFT))
    {
        DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&this->up);
        pos -= up * moveSpeed * dt;
        DirectX::XMStoreFloat3(&position, pos);
        bViewDirty = true;
    }
}

void Camera6DOF::OnMouseMove(int x, int y)
{
    if (!bFocused)
        return;

    int deltaX = x - centerPos.x;
    int deltaY = y - centerPos.y;

    if (deltaX != 0 || deltaY != 0)
    {
        pitch -= deltaY * mouseSensitivity;
        yaw += deltaX * mouseSensitivity;
        bViewDirty = true;

        HWND hWnd = GetActiveWindow();
        POINT screenCenter = centerPos;
        ClientToScreen(hWnd, &screenCenter);
        SetCursorPos(screenCenter.x, screenCenter.y);
    }
}

void Camera6DOF::UpdateVectors()
{
    pitch = std::clamp(pitch, -89.0f * 3.14159f / 180.0f, 89.0f * 3.14159f / 180.0f);

    DirectX::XMVECTOR forward =
        DirectX::XMVectorSet(sinf(yaw) * cosf(pitch), sinf(pitch), cosf(yaw) * cosf(pitch), 0.0f);
    forward = DirectX::XMVector3Normalize(forward);
    DirectX::XMStoreFloat3(&this->forward, forward);

    DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR right = DirectX::XMVector3Cross(worldUp, forward);
    right = DirectX::XMVector3Normalize(right);
    DirectX::XMStoreFloat3(&this->right, right);

    DirectX::XMVECTOR up = DirectX::XMVector3Cross(forward, right);
    up = DirectX::XMVector3Normalize(up);
    DirectX::XMStoreFloat3(&this->up, up);

    bViewDirty = true;
}

DirectX::XMMATRIX Camera6DOF::GetViewMatrix() const
{
    if (bViewDirty)
    {
        DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR target = pos + DirectX::XMLoadFloat3(&forward);
        DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&this->up);
        viewMatrix = DirectX::XMMatrixLookAtLH(pos, target, up);
        bViewDirty = false;
    }
    return viewMatrix;
}

DirectX::XMMATRIX Camera6DOF::GetProjectionMatrix(float aspectRatio) const
{
    return DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera6DOF::OnMouseWheel(int delta)
{
    fov = std::clamp(fov - delta * zoomSpeed * 0.01f, 1.0f, 120.0f);
}

void Camera6DOF::OnKeyDown(WPARAM key)
{
}

void Camera6DOF::OnKeyUp(WPARAM key)
{
}

void Camera6DOF::OnRightMouseDown()
{
}

void Camera6DOF::OnRightMouseUp()
{
}

void Camera6DOF::SetPosition(float x, float y, float z)
{
    position = {x, y, z};
    bViewDirty = true;
}

void Camera6DOF::SetPosition(const DirectX::XMFLOAT3& pos)
{
    position = pos;
    bViewDirty = true;
}

void Camera6DOF::SetRotation(float pitch, float yaw)
{
    this->pitch = pitch;
    this->yaw = yaw;
    bViewDirty = true;
}

void Camera6DOF::LookAt(const DirectX::XMFLOAT3& target)
{
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR tgt = DirectX::XMLoadFloat3(&target);
    DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(tgt, pos);
    dir = DirectX::XMVector3Normalize(dir);

    float x = DirectX::XMVectorGetX(dir);
    float y = DirectX::XMVectorGetY(dir);
    float z = DirectX::XMVectorGetZ(dir);

    yaw = atan2f(x, z);
    pitch = asinf(y);
    bViewDirty = true;
}

void Camera6DOF::Reset()
{
    position = {0.0f, 0.0f, -5.0f};
    pitch = 0.0f;
    yaw = 0.0f;
    fov = 45.0f;
    bFocused = false;
    bViewDirty = true;

    ShowCursor(TRUE);
    ClipCursor(nullptr);
}
