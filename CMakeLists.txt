project(network)

add_library(network SHARED Network.cpp NewtonRaphson.cpp Solver.cpp NetworkSolver.cpp ConnectedComponents.cpp)

target_link_libraries(network armadillo)

