/*/
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Atom/Feature/TransformService/TransformServiceFeatureProcessorInterface.h>
#include <Atom/RHI/ImageView.h>
#include <Atom/RPI.Public/Material/Material.h>
#include <AzCore/Math/Color.h>

namespace AZ::Render
{
    // FallbackPBR provides a common set of material parameters for each mesh in the scene, independent of the assigned material.
    // The parameters are estimated from the existing material-types (StandardPBR, EnhancedPBR, etc) using some heuristics, but that may
    // fail with more custom materials.
    //
    // See also the ShaderLib files Atom/Features/FallbackPBRMaterial/*.azsli
    //
    // This exists for two reasons:
    // - Access to all material parameters from a single draw-call:
    //     Compute Passes, Fullscreen Triangle passes and Raytracing Hit Shaders sometimes need access to the materials of any mesh in the
    //     scene, but only use one draw call (or one hit shader). This would mean a single shader needs to support multiple material types
    //     at once, which is generally not possible. Instead they can access the FallbackPBR material parametes, which are less detailled
    //     but still useful in many cases, e.g. GI solutions usually don't need the exact material parameters to produce reasonable results.
    // - backwards - compatability with existing raytracing implementations:
    //     This was previously part of the RayTracingFeatureProcessor and the only method to provide material info to the
    //     Hit-Shader of the Raytracing Pass.
    namespace FallbackPBR
    {
        // These parameters will be converted into the MaterialInfo struct on the GPU
        struct MaterialParameters
        {
            // color of the bounced light from this sub-mesh
            AZ::Color m_irradianceColor = AZ::Color(1.0f);

            // material data
            AZ::Color m_baseColor = AZ::Color(0.0f);
            float m_metallicFactor = 0.0f;
            float m_roughnessFactor = 0.0f;
            AZ::Color m_emissiveColor = AZ::Color(0.0f);

            // material textures
            RHI::Ptr<const RHI::ImageView> m_baseColorImageView;
            RHI::Ptr<const RHI::ImageView> m_normalImageView;
            RHI::Ptr<const RHI::ImageView> m_metallicImageView;
            RHI::Ptr<const RHI::ImageView> m_roughnessImageView;
            RHI::Ptr<const RHI::ImageView> m_emissiveImageView;
        };

        // One entry for each mesh in the scene. These use the same indices as the MeshInfo - Array
        struct MaterialEntry : public AZStd::intrusive_base
        {
            TransformServiceFeatureProcessorInterface::ObjectId m_objectId;
            Data::Instance<RPI::Material> m_material;
            RPI::Material::ChangeId m_materialChangeId;
            MaterialParameters m_materialParameters;
        };
    } // namespace FallbackPBR

} // namespace AZ::Render
