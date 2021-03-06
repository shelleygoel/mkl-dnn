/*******************************************************************************
* Copyright 2016 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef POOLING_PD_HPP
#define POOLING_PD_HPP

#include "mkldnn.h"

#include "c_types_map.hpp"
#include "primitive_desc.hpp"
#include "memory_pd.hpp"

namespace mkldnn {
namespace impl {

struct pooling_fwd_pd_t: public primitive_desc_t {
    typedef pooling_fwd_pd_t base_class;
    typedef pooling_fwd_pd_t hint_class;
    static constexpr auto base_pkind = primitive_kind::pooling;

    pooling_fwd_pd_t(mkldnn::impl::engine_t *engine,
            const pooling_desc_t *adesc, const pooling_fwd_pd_t *hint_fwd_pd)
        : primitive_desc_t(engine, primitive_kind::pooling)
        , desc_(*adesc), hint_fwd_pd_(hint_fwd_pd) {}
    virtual ~pooling_fwd_pd_t() {}

    const pooling_desc_t *desc() const { return &desc_; }
    virtual const op_desc_t *op_desc() const override
    { return reinterpret_cast<const op_desc_t *>(this->desc()); }

    virtual const memory_pd_t *input_pd(int index = 0) const override
    { return index == 0 ? src_pd() : nullptr; }
    virtual const memory_pd_t *output_pd(int index = 0) const override {
        if (index == 0) return dst_pd();
        if (index == 1) return workspace_pd();
        return nullptr;
    }

    virtual int n_inputs() const override { return 1; }
    virtual int n_outputs() const override
    { return 1 + (workspace_pd() != nullptr); }

    virtual status_t query(query_t what, int idx, void *result) const override
    {
        switch (what) {
        case query::pooling_d:
            *(const pooling_desc_t**)result = desc(); break;
        default: return primitive_desc_t::query(what, idx, result);
        }
        return status::success;
    }

    /* common pooling aux functions */

    inline int MB() const { return desc_.src_desc.dims[0]; }
    inline int C() const { return desc_.src_desc.dims[1]; }
    inline int IH() const { return desc_.src_desc.dims[2]; }
    inline int IW() const { return desc_.src_desc.dims[3]; }
    inline int OH() const { return desc_.dst_desc.dims[2]; }
    inline int OW() const { return desc_.dst_desc.dims[3]; }
    inline int KH() const { return desc_.kernel[0]; }
    inline int KW() const { return desc_.kernel[1]; }

    inline int KSH() const { return desc_.strides[0]; }
    inline int KSW() const { return desc_.strides[1]; }

    inline int padT() const { return desc_.padding[0][0]; }
    inline int padB() const { return desc_.padding[1][0]; }
    inline int padL() const { return desc_.padding[0][1]; }
    inline int padR() const { return desc_.padding[1][1]; }


protected:
    pooling_desc_t desc_;
    const pooling_fwd_pd_t *hint_fwd_pd_;
};

struct pooling_bwd_pd_t: public primitive_desc_t {
    typedef pooling_bwd_pd_t base_class;
    typedef pooling_fwd_pd_t hint_class;
    static constexpr auto base_pkind = primitive_kind::pooling;

    pooling_bwd_pd_t(mkldnn::impl::engine_t *engine,
            const pooling_desc_t *adesc, const pooling_fwd_pd_t *hint_fwd_pd)
        : primitive_desc_t(engine, primitive_kind::pooling)
        , desc_(*adesc), hint_fwd_pd_(hint_fwd_pd) {}
    virtual ~pooling_bwd_pd_t() {}

    const pooling_desc_t *desc() const { return &desc_; }
    virtual const op_desc_t *op_desc() const override
    { return reinterpret_cast<const op_desc_t *>(this->desc()); }

    virtual const memory_pd_t *input_pd(int index = 0) const override  {
        if (index == 0) return diff_dst_pd();
        if (index == 1) return workspace_pd();
        return nullptr;
    }
    virtual const memory_pd_t *output_pd(int index = 0) const override
    { return index == 0 ? diff_src_pd() : nullptr; }

    virtual int n_inputs() const override
    { return 1 + (workspace_pd() != nullptr); }
    virtual int n_outputs() const override { return 1; }

    virtual status_t query(query_t what, int idx, void *result) const override
    {
        switch (what) {
        case query::pooling_d:
            *(const pooling_desc_t**)result = desc(); break;
        default: return primitive_desc_t::query(what, idx, result);
        }
        return status::success;
    }

    /* common pooling aux functions */

    inline int MB() const { return desc_.diff_src_desc.dims[0]; }
    inline int C() const { return desc_.diff_src_desc.dims[1]; }
    inline int IH() const { return desc_.diff_src_desc.dims[2]; }
    inline int IW() const { return desc_.diff_src_desc.dims[3]; }
    inline int OH() const { return desc_.diff_dst_desc.dims[2]; }
    inline int OW() const { return desc_.diff_dst_desc.dims[3]; }
    inline int KH() const { return desc_.kernel[0]; }
    inline int KW() const { return desc_.kernel[1]; }

    inline int KSH() const { return desc_.strides[0]; }
    inline int KSW() const { return desc_.strides[1]; }

    inline int padT() const { return desc_.padding[0][0]; }
    inline int padB() const { return desc_.padding[1][0]; }
    inline int padL() const { return desc_.padding[0][1]; }
    inline int padR() const { return desc_.padding[1][1]; }


protected:
    pooling_desc_t desc_;
    const pooling_fwd_pd_t *hint_fwd_pd_;
};

}
}

#endif

// vim: et ts=4 sw=4 cindent cino^=l0,\:0,N-s

