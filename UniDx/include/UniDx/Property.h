#pragma once

#include <functional>

//
// C#�̃v���p�e�B���C�N�ȋL�q����������N���X
// ReadOnlyProperty<>
// Property<>
// 
namespace UniDx
{

// �ǂݎ���p�v���p�e�B
template<typename T>
class ReadOnlyProperty
{
public:
    using Getter = std::function<T()>;

    // Getter��^����R���X�g���N�^
    ReadOnlyProperty(Getter getter)
        : getter_(getter) {
    }

    // �l�̎擾
    T get() const { return getter_(); }

    // �l�̕ϊ�
    operator T() const { return getter_(); }

protected:
    Getter getter_;
};

// �ǂݎ���p�v���p�e�B�|�C���^��
template<typename T>
class ReadOnlyProperty<T*>
{
public:
    using Getter = std::function< T* ()>;

    ReadOnlyProperty(Getter getter)
        : getter_(getter) {
    }

    // �l�̎擾
    T* get() const { return getter_(); }

    // �l�̕ϊ�
    operator T*() const { return getter_(); }

    // �����o�A�N�Z�X
    T* operator->() { return getter_(); }
    const T* operator->() const { return getter_(); }

protected:
    Getter getter_;
};

// �ǂݏ����v���p�e�B
template<typename T>
class Property : public ReadOnlyProperty<T>
{
public:
    using Getter = ReadOnlyProperty<T>::Getter;
    using Setter = std::function<void(const T&)>;

    Property(Getter getter, Setter setter)
        : ReadOnlyProperty<T>(getter), setter_(setter) {
    }

    // �l�̐ݒ�
    void set(const T& value) { setter_(value); }

    // C#���̃A�N�Z�X
    Property& operator=(const T& value) { set(value); return *this; }

private:
    Setter setter_;
};

}
