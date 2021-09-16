//
// Created by Zero on 13/09/2021.
//


#pragma once

#include "base_libs/header.h"
#include <map>
#include <string>
#include "reflection.h"
#include "core/macro_map.h"

namespace luminous {

    inline namespace refl {
        struct TypeData {

            const size_t size;
            const size_t align;

            explicit TypeData(size_t size = 0, size_t align = 0)
                    : size(size), align(align) {}
        };

        class ClassFactory {
        private:
            static ClassFactory *_instance;

            std::map<std::string, TypeData> _type_data;

            ClassFactory() = default;

            ClassFactory(const ClassFactory &) = default;

            ClassFactory &operator=(const ClassFactory &) = default;

        public:
            static ClassFactory *instance();

            NDSC bool is_registered(const std::string &class_name) const;

            NDSC bool is_registered(const Object *object) const;

            NDSC size_t size_of(const std::string &class_name) const;

            NDSC size_t runtime_size_of(const Object *object) const;

            NDSC size_t size_of(const Object *object) const;

            NDSC size_t align_of(const std::string &class_name) const;

            NDSC size_t align_of(const Object *object) const;

            NDSC TypeData type_data(const Object *object) const;

            NDSC TypeData type_data(const std::string &class_name) const;

            NDSC size_t type_num() const { return _type_data.size(); }

            void register_class(const std::string &class_name, const TypeData &type_data);

            template<typename T>
            void register_class() {
                auto class_name = type_name<T>();
                TypeData type_data(sizeof(T), alignof(T));
                register_class(class_name, type_data);
            }

            template<typename ...Args>
            size_t size_sum(Args &&...args) {

            }
        };


        class Object {
            REFL_CLASS(Object)
        protected:
            std::unique_ptr<Object*> _objects;
        public:
            using Object_ptr = Object *;

            using PtrMapper = std::map<Object *, size_t>;

            NDSC virtual const char *class_name() const {
                return type_name(this);
            }

            NDSC virtual size_t self_size() const {
                return ClassFactory::instance()->size_of(this);
            }

            NDSC virtual size_t ptr_member_size() const {
                size_t size = 0;

                return size;
            }

            NDSC virtual size_t real_size() const {
                return self_size() + ptr_member_size();
            }

            template<typename ...Args>
            static size_t mapping_ptr(PtrMapper &mapper, size_t offset, Args &&...ptr) {
                ((mapper[ptr] = offset, offset += ptr->mapping_member_ptr(mapper, offset)), ...);
                return offset;
            }

            virtual size_t mapping_member_ptr(PtrMapper &mapper, size_t offset) {
                mapper[this] = offset;
                offset += self_size();
                return offset;
            }
        };

        template<typename T>
        class RegisterAction {
        public:
            RegisterAction() {
                ClassFactory::instance()->template register_class<T>();
            }
        };

#define REGISTER(T) RegisterAction<T> Register##T;
    }

}