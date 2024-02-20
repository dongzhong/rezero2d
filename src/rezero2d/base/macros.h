// Created by DONG Zhong on 2024/02/06.

#ifndef REZERO_BASE_MACROS_H_
#define REZERO_BASE_MACROS_H_

#define REZERO_DISALLOW_COPY(TypeName) TypeName(const TypeName&) = delete

#define REZERO_DISALLOW_ASSIGN(TypeName) TypeName& operator=(const TypeName&) = delete

#define REZERO_DISALLOW_MOVE(TypeName) \
  TypeName(TypeName&&) = delete;       \
  TypeName& operator=(TypeName&&) = delete

#define REZERO_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;             \
  TypeName& operator=(const TypeName&) = delete

#define REZERO_DISALLOW_COPY_ASSIGN_AND_MOVE(TypeName) \
  TypeName(const TypeName&) = delete;                  \
  TypeName(TypeName&&) = delete;                       \
  TypeName& operator=(const TypeName&) = delete;       \
  TypeName& operator=(TypeName&&) = delete

#endif // REZERO_BASE_MACROS_H_
