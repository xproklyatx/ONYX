#include "Camera6DOF.hpp"
#include "DeltaTime.hpp"
#include "Input.hpp"
#include <algorithm>
#include <cmath>

using namespace DirectX;

Camera6DOF::Camera6DOF()
    : position{0.0f, 0.0f, -5.0f}, forward{0.0f, 0.0f, 1.0f}, right{1.0f, 0.0f, 0.0f}, up{0.0f, 1.0f, 0.0f},
      pitch(0.0f), yaw(0.0f), fov(45.0f), bFocused(false), bViewDirty(true), centerPos{0, 0}, lastMousePos{0, 0}
{
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

    XMVECTOR pos = XMLoadFloat3(&position);
    XMVECTOR fwd = XMLoadFloat3(&forward);
    XMVECTOR rightVec = XMLoadFloat3(&right);
    XMVECTOR upVec = XMLoadFloat3(&up);

    float speed = moveSpeed * dt;

    if (Input::IsKeyDown('W') || Input::IsKeyDown('w'))
    {
        pos = XMVectorAdd(pos, XMVectorScale(fwd, speed));
        bViewDirty = true;
    }
    if (Input::IsKeyDown('S') || Input::IsKeyDown('s'))
    {
        pos = XMVectorSubtract(pos, XMVectorScale(fwd, speed));
        bViewDirty = true;
    }
    if (Input::IsKeyDown('A') || Input::IsKeyDown('a'))
    {
        pos = XMVectorSubtract(pos, XMVectorScale(rightVec, speed));
        bViewDirty = true;
    }
    if (Input::IsKeyDown('D') || Input::IsKeyDown('d'))
    {
        pos = XMVectorAdd(pos, XMVectorScale(rightVec, speed));
        bViewDirty = true;
    }
    if (Input::IsKeyDown(VK_SPACE))
    {
        pos = XMVectorAdd(pos, XMVectorScale(upVec, speed));
        bViewDirty = true;
    }
    if (Input::IsKeyDown(VK_SHIFT))
    {
        pos = XMVectorSubtract(pos, XMVectorScale(upVec, speed));
        bViewDirty = true;
    }

    XMStoreFloat3(&position, pos);
}

void Camera6DOF::OnMouseMove(int x, int y)
{
    if (!bFocused)
        return;

    int deltaX = x - centerPos.x;
    int deltaY = y - centerPos.y;

    if (deltaX != 0 || deltaY != 0)
    {
        pitch -= static_cast<float>(deltaY) * mouseSensitivity;
        yaw += static_cast<float>(deltaX) * mouseSensitivity;
        bViewDirty = true;

        HWND hWnd = GetActiveWindow();
        POINT screenCenter = centerPos;
        ClientToScreen(hWnd, &screenCenter);
        SetCursorPos(screenCenter.x, screenCenter.y);
    }
}

void Camera6DOF::UpdateVectors()
{
    const float pitchLimit = 89.0f * 3.14159f / 180.0f;
    pitch = std::clamp(pitch, -pitchLimit, pitchLimit);

    XMVECTOR fwd = XMVectorSet(sinf(yaw) * cosf(pitch), sinf(pitch), cosf(yaw) * cosf(pitch), 0.0f);
    fwd = XMVector3Normalize(fwd);
    XMStoreFloat3(&forward, fwd);

    XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR rightVec = XMVector3Cross(worldUp, fwd);
    rightVec = XMVector3Normalize(rightVec);
    XMStoreFloat3(&right, rightVec);

    XMVECTOR upVec = XMVector3Cross(fwd, rightVec);
    upVec = XMVector3Normalize(upVec);
    XMStoreFloat3(&up, upVec);

    bViewDirty = true;
}

DirectX::XMMATRIX Camera6DOF::GetViewMatrix() const
{
    if (bViewDirty)
    {
        XMVECTOR pos = XMLoadFloat3(&position);
        XMVECTOR target = XMVectorAdd(pos, XMLoadFloat3(&forward));
        XMVECTOR upVec = XMLoadFloat3(&up);
        viewMatrix = XMMatrixLookAtLH(pos, target, upVec);
        bViewDirty = false;
    }
    return viewMatrix;
}

DirectX::XMMATRIX Camera6DOF::GetProjectionMatrix(float aspectRatio) const
{
    return XMMatrixPerspectiveFovLH(XMConvertToRadians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera6DOF::OnMouseWheel(int delta)
{
    fov = std::clamp(fov - static_cast<float>(delta) * zoomSpeed * 0.01f, 1.0f, 120.0f);
}

void Camera6DOF::OnKeyDown(WPARAM key)
{ /* Not implemented */
}
void Camera6DOF::OnKeyUp(WPARAM key)
{ /* Not implemented */
}
void Camera6DOF::OnRightMouseDown()
{ /* Not implemented */
}
void Camera6DOF::OnRightMouseUp()
{ /* Not implemented */
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
    XMVECTOR pos = XMLoadFloat3(&position);
    XMVECTOR tgt = XMLoadFloat3(&target);
    XMVECTOR dir = XMVectorSubtract(tgt, pos);
    dir = XMVector3Normalize(dir);

    float x = XMVectorGetX(dir);
    float y = XMVectorGetY(dir);
    float z = XMVectorGetZ(dir);

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
