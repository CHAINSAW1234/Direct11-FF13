#pragma once

namespace Engine
{
	template<typename T>
	void Safe_Delete(T& pPoint)
	{
		if (nullptr != pPoint)
		{
			delete pPoint;
			pPoint = nullptr;
		}	
	}

	template<typename T>
	void Safe_Delete_Array(T& pPoint)
	{
		if (nullptr != pPoint)
		{
			delete[] pPoint;
			pPoint = nullptr;
		}
	}


	template<typename T>
	unsigned int Safe_AddRef(T& pInstance)
	{
		unsigned int iRefCnt = 0;

		if (nullptr != pInstance)		
			iRefCnt = pInstance->AddRef();

		return iRefCnt;
	}

	template<typename T>
	unsigned int Safe_Release(T& pInstance)
	{
		unsigned int iRefCnt = 0;

		if (nullptr != pInstance)
		{
			iRefCnt = pInstance->Release();

			if(0 == iRefCnt)
				pInstance = nullptr;
		}

		return iRefCnt;
	}

	inline _float Cal_Degree_From_Directions_Between_Min180_To_180(_float4 vDir1, _float4 vDir2) {
		// 1. ����ȭ �� �� ���͸� ������ ������ ��� �ϱ�
		_float fRadian = XMVector3AngleBetweenNormals(XMLoadFloat4(&vDir1), XMLoadFloat4(&vDir2)).m128_f32[0];

		// �������� ���� ã��
		_float  vCross = XMVector3Cross(XMLoadFloat4(&vDir1), XMLoadFloat4(&vDir2)).m128_f32[1];

		// ������ -180 ~ 180�� ���̷� ����
		_float fDegree = XMConvertToDegrees(fRadian * vCross / abs(vCross)); // 0 ~ 180 ������ �� 
		// ������ 0���� ũ�� �����̴�
		return fDegree;
	}

}

