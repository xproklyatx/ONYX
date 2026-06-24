#pragma once
#include <DirectXMath.h>
#include <windows.h>

class ICam
{
  public:
    virtual ~ICam() = default;

    ICam(const ICam&) = delete;
    ICam& operator=(const ICam&) = delete;

    static ICam& GetInstance();
    static ICam* GetInstancePtr()
    {
        return pInstance;
    }
    static void Set(ICam* pCam);

    virtual void Update() = 0;
    virtual DirectX::XMMATRIX GetViewMatrix() const = 0;
    virtual DirectX::XMMATRIX GetProjectionMatrix(float aspectRatio) const = 0;

    virtual void OnMouseMove(int x, int y) = 0;
    virtual void OnMouseWheel(int delta) = 0;
    virtual void OnKeyDown(WPARAM key) = 0;
    virtual void OnKeyUp(WPARAM key) = 0;
    virtual void OnRightMouseDown() = 0;
    virtual void OnRightMouseUp() = 0;

  protected:
    ICam() = default;

  private:
    static ICam* pInstance;
};
