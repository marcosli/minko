/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once

#include "minko/Common.hpp"
#include "minko/file/AbstractParser.hpp"
#include "minko/file/FileProtocol.hpp"
#include "minko/component/Skinning.hpp"
#include "minko/render/Blending.hpp"

struct    aiMesh;
struct    aiNode;
struct    aiBone;
struct    aiScene;
struct    aiAnimation;
struct    aiNodeAnim;
struct    aiVectorKey;
struct    aiQuatKey;
template <typename> class aiVector3t;
template <typename> class aiQuaterniont;
template <typename> class aiMatrix4x4t;
struct    aiMaterial;

namespace Assimp
{
    class Importer;
}

namespace minko
{
    namespace geometry
    {
        class Bone;
        class Skin;
    }

    namespace file
    {
        class AbstractASSIMPParser :
            public AbstractParser
        {
        public:
            typedef std::shared_ptr<AbstractASSIMPParser>                        Ptr;

        private:
            typedef std::shared_ptr<Loader>                                        LoaderPtr;
            typedef std::shared_ptr<scene::Node>                                NodePtr;
            typedef std::shared_ptr<component::SceneManager>                    SceneManagerPtr;
            typedef std::shared_ptr<geometry::Geometry>                            GeometryPtr;
            typedef std::shared_ptr<geometry::Bone>                                BonePtr;
            typedef std::shared_ptr<geometry::Skin>                                SkinPtr;
            typedef std::shared_ptr<math::Vector3>                                Vector3Ptr;
            typedef std::shared_ptr<math::Vector4>                                Vector4Ptr;
            typedef std::shared_ptr<math::Quaternion>                            QuaternionPtr;
            typedef std::shared_ptr<math::Matrix4x4>                            Matrix4x4Ptr;
            typedef std::shared_ptr<material::Material>                            MaterialPtr;
            typedef std::shared_ptr<render::Effect>                                EffectPtr;

            typedef std::vector<Matrix4x4Ptr>                                    Matrices4x4;

            typedef Signal<LoaderPtr>::Slot                                        LoaderCompleteSignalSlot;
            typedef Signal<LoaderPtr, const file::Error&>::Slot            LoaderErrorSignalSlot;

            typedef std::unordered_map<LoaderPtr, LoaderCompleteSignalSlot>        LoaderToCompleteSlotMap;
            typedef std::unordered_map<LoaderPtr, LoaderErrorSignalSlot>        LoaderToErrorSlotMap;
            typedef std::unordered_map<uint, std::string>                        TextureTypeToName;

        private:
            static const TextureTypeToName                            _textureTypeToName;
            static const std::string                                PNAME_TRANSFORM;

            unsigned int                                            _numDependencies;
            unsigned int                                            _numLoadedDependencies;
            std::string                                                _filename;
            std::shared_ptr<AssetLibrary>                            _assetLibrary;
            std::shared_ptr<file::Options>                            _options;

            NodePtr                                                    _symbol;

            std::unordered_map<const aiNode*, NodePtr>                _aiNodeToNode;
            std::unordered_map<const aiMesh*, NodePtr>                _aiMeshToNode;
            std::unordered_map<std::string, NodePtr>                _nameToNode;
            std::unordered_map<std::string, Matrices4x4>            _nameToAnimMatrices;
            std::set<NodePtr>                                        _alreadyAnimatedNodes;

            std::set<std::string>                                   _meshNames;

            LoaderToCompleteSlotMap                                 _loaderCompleteSlots;
            LoaderToErrorSlotMap                                    _loaderErrorSlots;

            Assimp::Importer*                                       _importer;

        public:

            virtual ~AbstractASSIMPParser();

            void
            parse(const std::string&                filename,
                  const std::string&                resolvedFilename,
                  std::shared_ptr<Options>          options,
                  const std::vector<unsigned char>&    data,
                  std::shared_ptr<AssetLibrary>        assetLibrary);

        protected:

            AbstractASSIMPParser();

        private:

            virtual void provideLoaders(Assimp::Importer& importer) = 0;

            static
            TextureTypeToName
            initializeTextureTypeToName();

            void
            initImporter();

            void
            resetParser();

            void
            createSceneTree(NodePtr minkoNode, const aiScene* scene, aiNode* ainode, std::shared_ptr<AssetLibrary> assets);

            GeometryPtr
            createMeshGeometry(NodePtr, aiMesh*, const std::string&);

            static
            std::string
            getMeshName(const std::string& meshName);

            std::shared_ptr<component::Transform>
            getTransformFromAssimp(aiNode* ainode);

            void
            createMeshSurface(NodePtr, const aiScene*, aiMesh*);

            void
            createLights(const aiScene*);

            void
            createCameras(const aiScene*);

            void
            createAnimations(const aiScene*, bool interpolate = false);

            NodePtr
            findNode(const std::string&) const;

            void
            parseDependencies(const std::string& filename, const aiScene* scene);

            void
            finalize();

            void
            loadTexture(const std::string&                textureFilename,
                        const std::string&                assetName,
                        std::shared_ptr<file::Options>    options,
                        const aiScene*                    scene);

            void
            createSkins(const aiScene*);

            void
            createSkin(const aiMesh*);

            BonePtr
            createBone(const aiBone*) const;

            NodePtr
            getSkeletonRoot(const aiMesh*) const;

            NodePtr
            getBoneCommonAncestor(const aiMesh*) const;

            unsigned int
            getSkinNumFrames(const aiMesh*) const;

            void
            precomputeModelToRootMatrices(NodePtr node, NodePtr root, std::vector<Matrix4x4Ptr>&) const;

            void
            sampleAnimations(const aiScene*);

            void
            sampleAnimation(const aiAnimation*);

            static
            Vector3Ptr
            sample(const aiVectorKey*, const std::vector<float>&, float, Vector3Ptr output = nullptr);

            static
            QuaternionPtr
            sample(const aiQuatKey*, const std::vector<float>&, float, QuaternionPtr output = nullptr);

            static
            void
            sample(const aiNodeAnim*, const std::vector<float>&, std::vector<Matrix4x4Ptr>&);

            static
            Matrix4x4Ptr
            convert(const aiVector3t<float>&, const aiQuaterniont<float>&, const aiVector3t<float>&, Matrix4x4Ptr output = nullptr);

            static
            QuaternionPtr
            convert(const aiQuaterniont<float>&, QuaternionPtr output = nullptr);

            template<class AiKey>
            static
            void
            computeTimeFactors(unsigned int, const AiKey*, std::vector<float>&);

            template<class AiKey>
            static
            unsigned int
            getIndexForTime(unsigned int, const AiKey*, double);

            static
            Matrix4x4Ptr
            convert(const aiMatrix4x4t<float>&, Matrix4x4Ptr output = nullptr);

            MaterialPtr
            createMaterial(const aiMaterial*);

            MaterialPtr
            chooseMaterialByShadingMode(const aiMaterial*) const;

            EffectPtr
            chooseEffectByShadingMode(const aiMaterial*) const;

            render::Blending::Mode
            getBlendingMode(const aiMaterial*) const;

            render::TriangleCulling
            getTriangleCulling(const aiMaterial*) const;

            bool
            getWireframe(const aiMaterial*) const;

            float
            setScalarProperty(MaterialPtr, const std::string& propertyName, const aiMaterial*, const char*, unsigned int, unsigned int, float);

            Vector4Ptr
            setColorProperty(MaterialPtr, const std::string& propertyName, const aiMaterial*, const char*, unsigned int, unsigned int, Vector4Ptr);

            void
            disposeNodeMaps();

            void
            apply(NodePtr, const std::function<NodePtr(NodePtr)>&);

            void
            allDependenciesLoaded(const aiScene* scene);

            void
            textureCompleteHandler(std::shared_ptr<file::Loader> loader, const aiScene* scene);
        };
    }
}
