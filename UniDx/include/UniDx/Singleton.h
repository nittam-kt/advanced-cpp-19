#pragma once

#include <memory>
#include <assert.h>
#include "UniDxDefine.h"

namespace UniDx
{

// --------------------
// Singleton
//
// �������Ɣj���̃^�C�~���O�𐧌䂵����A
// ��̃N���X�� create �ł���悤�ɂ��邽��
// �����I�� create �� destroy ���K�v
// --------------------
template<class T>
class Singleton
{
public:
    // �C���X�^���X�̎擾
    static T* getInstance() { return instance_.get(); }

    // ���̃N���X���C���X�^���X�Ƃ��č쐬
    static void create()
    {
		assert(instance_ == nullptr);
        instance_ = std::make_unique<T>();
    }

    // �C���X�^���X�̔j��
	static void destroy()
	{
		instance_ = nullptr;
	}

protected:
    Singleton() {}
    virtual ~Singleton() {}

    static unique_ptr<T> instance_;
};

template<class T>
inline std::unique_ptr<T> Singleton<T>::instance_ = nullptr;

}