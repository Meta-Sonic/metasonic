#pragma once
#include "mts/config.h"
#include "mts/graphics/rect.h"

#include <CoreGraphics/CoreGraphics.h>

MTS_BEGIN_NAMESPACE

template <typename RectType>
static inline CGRect to_cg_rect(const RectType& r) {
  return CGRectMake((CGFloat)r.x, (CGFloat)r.y, (CGFloat)r.width, (CGFloat)r.height);
}

template <typename RectType>
static inline RectType from_cg_rect(const CGRect& r) {
    using T = typename RectType::value_type;
  return RectType((T)r.origin.x, (T)r.origin.y, (T)r.size.width, (T)r.size.height);
}


template <typename PointType>
static inline CGPoint to_cg_point(const PointType& p) {
  return CGPointMake((CGFloat)p.x, (CGFloat)p.y);
}

template <typename PointType>
static inline PointType from_cg_point(const CGPoint& p) {
  return PointType((typename PointType::value_type)p.x, (typename PointType::value_type)p.y);
}


template <typename SizeType>
static inline CGSize to_cg_size(const SizeType& s) {
  return CGSizeMake((CGFloat)s.width, (CGFloat)s.height);
}

template <typename SizeType>
static inline SizeType from_cg_size(const CGSize& s) {
  return SizeType((typename SizeType::value_type)s.width, (typename SizeType::value_type)s.height);
}

MTS_END_NAMESPACE
