#include "ConfigManager.hpp"


#include <vector>
#include <string>


ConfigManager::ConfigManager(FilepathManager* inFilepathManager){
    myFilepathManager = inFilepathManager;
    /*myModels.emplace_back("monkey_head.gltf" );
    myModels.emplace_back("tile.gltf" );
    myModels.emplace_back("evergreen_tree.gltf" );
    myModels.emplace_back("evergreen_tree_textured.gltf" );
    myModels.emplace_back("BarramundiFish.gltf" );
    myModels.emplace_back("cubes.gltf" );
    myModels.emplace_back("cube_face_two.gltf" );
    myModels.emplace_back("cube_face_uni.gltf" );
    myModels.emplace_back("cubes_gb.gltf" );*/
    //myModels.emplace_back("boolean_mats.gltf" );
    myModels.emplace_back("threefer.gltf" );
    /*myModels.emplace_back("evergreen_tree_one_node_test.gltf" );*/
    
    
    myShaders.emplace_back("simple_light.shaderxml");
    myShaders.emplace_back("simple_light_color.shaderxml");
    myShaders.emplace_back("plain_materials.shaderxml");
    myShaders.emplace_back("cel.shaderxml");
    
    myDefaultShader.assign("Simple Light");

    myScenes.emplace_back( "testing.scenexml" );
    myScenes.emplace_back( "default.scenexml" );
    myDefaultScene.assign("testing.scenexml");
}