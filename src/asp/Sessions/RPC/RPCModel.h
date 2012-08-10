// __BEGIN_LICENSE__
//  Copyright (c) 2009-2012, United States Government as represented by the
//  Administrator of the National Aeronautics and Space Administration. All
//  rights reserved.
//
//  The NGT platform is licensed under the Apache License, Version 2.0 (the
//  "License"); you may not use this file except in compliance with the
//  License. You may obtain a copy of the License at
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
// __END_LICENSE__

// RPC model and triangulation. Following the paper:

// Jacek Grodecki, Gene Dial and James Lutes, "Mathematical Model for
// 3D Feature Extraction from Multiple Satellite Images Described by
// RPCs." Proceedings of ASPRS 2004 Conference, Denver, Colorado, May,
// 2004.
  
#ifndef __STEREO_SESSION_RPC_MODEL_H__
#define __STEREO_SESSION_RPC_MODEL_H__

#include <vw/FileIO/DiskImageResourceGDAL.h>
#include <vw/Camera/CameraModel.h>
#include <vw/Cartography/Datum.h>

namespace asp {

  class RPCModel : public vw::camera::CameraModel {
    vw::cartography::Datum m_datum;

    // Scaling parameters
    vw::Vector<double,20> m_line_num_coeff, m_line_den_coeff,
      m_sample_num_coeff, m_sample_den_coeff;
    vw::Vector2 m_xy_offset;
    vw::Vector2 m_xy_scale;
    vw::Vector3 m_lonlatheight_offset;
    vw::Vector3 m_lonlatheight_scale;

    void initialize( vw::DiskImageResourceGDAL* resource );
  public:
    RPCModel( std::string const& filename );
    RPCModel( vw::DiskImageResourceGDAL* resource );
    RPCModel( vw::cartography::Datum const& datum,
              vw::Vector<double,20> const& line_num_coeff,
              vw::Vector<double,20> const& line_den_coeff,
              vw::Vector<double,20> const& samp_num_coeff,
              vw::Vector<double,20> const& samp_den_coeff,
              vw::Vector2 const& xy_offset,
              vw::Vector2 const& xy_scale,
              vw::Vector3 const& lonlatheight_offset,
              vw::Vector3 const& lonlatheight_scale );

    virtual std::string type() const { return "RPC"; }
    virtual ~RPCModel() {}

    // Standard Access Methods (Most of these will fail because they
    // don't apply well to RPC.)
    virtual vw::Vector2 point_to_pixel( vw::Vector3 const& point ) const;
    virtual vw::Vector3 pixel_to_vector( vw::Vector2 const& /*pix*/ ) const {
      vw::vw_throw( vw::NoImplErr() << "RPCModel: Pixel to Vector not implemented" );
      return vw::Vector3();
    }
    virtual vw::Vector3 camera_center( vw::Vector2 const& /*pix*/ ) const {
      vw::vw_throw( vw::NoImplErr() << "RPCModel: Camera center not implemented" );
      return vw::Vector3();
    }
    vw::Vector2 geodetic_to_pixel( vw::Vector3 const& point ) const;

    // Access to constants
    typedef vw::Vector<double,20> CoeffVec;
    vw::cartography::Datum const& datum() const { return m_datum; }
    CoeffVec const& line_num_coeff() const   { return m_line_num_coeff; }
    CoeffVec const& line_den_coeff() const   { return m_line_den_coeff; }
    CoeffVec const& sample_num_coeff() const { return m_sample_num_coeff; }
    CoeffVec const& sample_den_coeff() const { return m_sample_den_coeff; }
    vw::Vector2 const& xy_offset() const     { return m_xy_offset; }
    vw::Vector2 const& xy_scale() const      { return m_xy_scale; }
    vw::Vector3 const& lonlatheight_offset() const { return m_lonlatheight_offset; }
    vw::Vector3 const& lonlatheight_scale() const  { return m_lonlatheight_scale; }

    // Helper methods used for triangulation and projection
    static CoeffVec calculate_terms( vw::Vector3 const& v );
    static vw::Matrix<double, 20, 3> terms_Jacobian( vw::Vector3 const& v );
    static CoeffVec quotient_Jacobian( CoeffVec const& c, CoeffVec const& d,
                                       CoeffVec const& u );
    static vw::Matrix3x3 normalization_Jacobian(vw::Vector3 const& q);

    vw::Matrix<double, 2, 3> geodetic_to_pixel_Jacobian( vw::Vector3 const& geodetic ) const;

    
#if 1
    void ctr_and_dir(vw::Vector2 const& pix, vw::Vector3 & ctr, vw::Vector3 & dir ) const;
    vw::Vector2 image_to_ground( vw::Vector2 const& observedPixel, double height ) const;
#endif
    
  };

  inline std::ostream& operator<<(std::ostream& os, const RPCModel& rpc) {
    os << "RPC Model:" << std::endl
       << "Line Numerator: " << rpc.line_num_coeff() << std::endl
       << "Line Denominator: " << rpc.line_den_coeff() << std::endl
       << "Samp Numerator: " << rpc.sample_num_coeff() << std::endl
       << "Samp Denominator: " << rpc.sample_den_coeff() << std::endl
       << "XY Offset: " << rpc.xy_offset() << std::endl
       << "XY Scale: " << rpc.xy_scale() << std::endl
       << "Geodetic Offset: " << rpc.lonlatheight_offset() << std::endl
       << "Geodetic Scale: " << rpc.lonlatheight_scale();
    return os;
  }
}

#endif//__STEREO_SESSION_RPC_CAMERA_MODEL_H__
