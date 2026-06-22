#pragma once
#include "ICam.hpp"
#include <DirectXMath.h>
#include <algorithm>
class Camera6DOF : public ICam
{
  public:
    Camera6DOF();
    virtual ~Camera6DOF() = default;

    virtual void Update() override;
    virtual DirectX::XMMATRIX GetViewMatrix() const override;
    virtual DirectX::XMMATRIX GetProjectionMatrix(float aspectRatio) const override;

    virtual void OnMouseMove(int x, int y) override;
    virtual void OnMouseWheel(int delta) override;
    virtual void OnKeyDown(WPARAM key) override;
    virtual void OnKeyUp(WPARAM key) override;
    virtual void OnRightMouseDown() override;
    virtual void OnRightMouseUp() override;

    void SetPosition(float x, float y, float z);
    void SetPosition(const DirectX::XMFLOAT3& pos);
    void SetRotation(float pitch, float yaw);
    void LookAt(const DirectX::XMFLOAT3& target);
    void Reset();

    DirectX::XMFLOAT3 GetPosition() const
    {
        return position;
    }
    DirectX::XMFLOAT3 GetForward() const
    {
        return forward;
    }
    DirectX::XMFLOAT3 GetUp() const
    {
        return up;
    }
    DirectX::XMFLOAT3 GetRight() const
    {
        return right;
    }
    float GetPitch() const
    {
        return pitch;
    }
    float GetYaw() const
    {
        return yaw;
    }
    float GetFov() const
    {
        return fov;
    }

    void SetSpeed(float speed)
    {
        moveSpeed = speed;
    }
    void SetSensitivity(float sensitivity)
    {
        mouseSensitivity = sensitivity;
    }
    void SetFov(float fov)
    {
        fov = std::clamp(fov, 1.0f, 120.0f);
    }

  private:
    void UpdateVectors();
    void ProcessInput();

    DirectX::XMFLOAT3 position = {0.0f, 0.0f, -5.0f};
    DirectX::XMFLOAT3 forward = {0.0f, 0.0f, 1.0f};
    DirectX::XMFLOAT3 up = {0.0f, 1.0f, 0.0f};
    DirectX::XMFLOAT3 right = {1.0f, 0.0f, 0.0f};

    float pitch = 0.0f;
    float yaw = 0.0f;

    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;

    float moveSpeed = 5.0f;
    float mouseSensitivity = 0.002f;
    float zoomSpeed = 2.0f;

    bool bRightMouseDown = false;
    POINT lastMousePos = {0, 0};

    mutable bool bViewDirty = true;
    mutable DirectX::XMMATRIX viewMatrix;
};
