# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.


Pod::Spec.new do |spec|
  spec.name = 'Yoga'
  spec.version = '3.2.2'
  spec.license =  { :type => 'MIT', :file => "LICENSE" }
  spec.homepage = 'https://yogalayout.dev/'
  spec.documentation_url = 'https://yogalayout.dev/docs'

  spec.summary = 'An embeddable and performant flexbox layout engine with bindings for multiple languages'

  spec.authors = {'Meta Open Source' => 'opensource@meta.com'}
  spec.source = {
    :git => 'https://github.com/facebook/yoga.git',
    :tag => "v#{spec.version.to_s}",
  }

  spec.ios.deployment_target = "13.4"

  spec.module_name = 'yoga'
  spec.requires_arc = false
  spec.pod_target_xcconfig = {
    'DEFINES_MODULE' => 'YES',
    'HEADER_SEARCH_PATHS' => '"$(PODS_TARGET_SRCROOT)"',
  }

  spec.compiler_flags = [
      '-fno-omit-frame-pointer',
      '-fexceptions',
      '-Wall',
      '-Werror',
      '-std=c++20',
      '-fPIC'
  ]

  spec.swift_version = '5.1'
  spec.source_files = 'yoga/**/*.{h,cpp}'
  spec.header_mappings_dir = 'yoga'

  public_header_files = 'yoga/*.h'
  spec.public_header_files = public_header_files

  all_header_files = 'yoga/**/*.h'
  spec.private_header_files = Dir.glob(all_header_files) - Dir.glob(public_header_files)
  spec.preserve_paths = [all_header_files]
end
