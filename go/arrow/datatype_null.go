// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package arrow

// NullType describes a degenerate array, with zero physical storage.
type NullType struct{}

func (*NullType) ID() Type            { return NULL }
func (*NullType) Name() string        { return "null" }
func (*NullType) String() string      { return "null" }
func (*NullType) Fingerprint() string { return typeIDFingerprint(NULL) }
func (*NullType) Layout() DataTypeLayout {
	return DataTypeLayout{Buffers: []BufferSpec{SpecAlwaysNull()}}
}

// Null gives us both the compile-time assertion of DataType interface as well as serving a good element for use in schemas.
var Null DataType = new(NullType)
