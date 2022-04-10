///
/// BSD 3-Clause License
///
/// Copyright (c) 2022, Alexandre Arsenault
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// * Redistributions of source code must retain the above copyright notice, this
///   list of conditions and the following disclaimer.
///
/// * Redistributions in binary form must reproduce the above copyright notice,
///   this list of conditions and the following disclaimer in the documentation
///   and/or other materials provided with the distribution.
///
/// * Neither the name of the copyright holder nor the names of its
///   contributors may be used to endorse or promote products derived from
///   this software without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
/// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
/// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
/// POSSIBILITY OF SUCH DAMAGE.
///

#pragma once
#include "mts/config.h"
#include "mts/audio/detail/vector_operations_list.h"

#if __MTS_USE_ACCELERATE__

MTS_BEGIN_SUB_NAMESPACE(vec)
struct vdsp_ops : base_ops {
  MTS_AUDIO_DECLARE_OP_FD(CLEAR);
  MTS_AUDIO_DECLARE_OP_FD(FILL);
  MTS_AUDIO_DECLARE_OP_FD(COPY);
  MTS_AUDIO_DECLARE_OP_FD(CLIP);
  MTS_AUDIO_DECLARE_OP_FD(ABS);

  MTS_AUDIO_DECLARE_OP_FD(SQRT);
  MTS_AUDIO_DECLARE_OP_FD(SIN);
  MTS_AUDIO_DECLARE_OP_FD(COS);
  MTS_AUDIO_DECLARE_OP_FD(TAN);
  MTS_AUDIO_DECLARE_OP_FD(TANH);

  MTS_AUDIO_DECLARE_OP_FD(S_ADD);
  MTS_AUDIO_DECLARE_OP_FD(S_MUL);
  MTS_AUDIO_DECLARE_OP_FD(S_DIV);
  MTS_AUDIO_DECLARE_OP_FD(S_VDIV);
};

using impl_ops = vdsp_ops;
struct ops_type : impl_ops {
  __MTS_AUDIO_OPS_DECLARE_USING();
};

MTS_END_SUB_NAMESPACE(vec)
#endif // __MTS_USE_ACCELERATE__
