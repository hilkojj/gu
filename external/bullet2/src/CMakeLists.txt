cmake_minimum_required(VERSION 3.0)
project(Bullet)


add_library(Bullet INTERFACE)
set(BULLET_VERSION 3.21_ea751cc)
set(BULLET_PHYSICS_SOURCE_DIR ../../)

add_subdirectory(BulletCollision ./bin/BulletCollision)

add_subdirectory(BulletDynamics ./bin/BulletDynamics)

#add_subdirectory(BulletSoftBody ./bin/BulletSoftBody)

add_subdirectory(LinearMath ./bin/LinearMath)

target_link_libraries(Bullet INTERFACE
	# order is important here: Dynamics needs Collision, and therefore this order is required somehow.
	BulletDynamics
	BulletCollision
	#BulletSoftBody
	LinearMath
)
target_include_directories(Bullet PUBLIC INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
