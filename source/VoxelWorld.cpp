#include "VoxelWorld.hpp"
#include "Vectors.hpp"
#include "Config.hpp"

#include <map>

#include <chrono>

namespace vox {

// start perlin stufff

using ui8 = uint8_t;
using ui32 = uint32_t;

// constexpr ui8 permutations[256] =
// {
//  151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225,
//  140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148,
//  247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32,
//  57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175,
//  74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122,
//  60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54,
//  65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169,
//  200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64,
//  52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212,
//  207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213,
//  119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
//  129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104,
//  218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241,
//  81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157,
//  184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93,
//  222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
// };

static float    prn(ui32& id)
{
    ui32        result;

    id = id * 747796405 + 2891336453;
    result = ((id >> ((id >> 28) + 4)) ^ id) * 277803737;
    result = (result >> 22) ^ result;
    return static_cast<float>(result / 4294967295.0);
}

// static float norm_dist(ui32& id)
// {
//  float   rho;
//  float   theta;

//  rho = std::sqrt(-2 * std::log(prn(id)));
//  theta = 2 * pi() * prn(id);
//  return (rho * std::cos(theta));
// }

// float    perlinNoise(float x, float y, ui32 seed)
// {
//  float xFloor = x - std::floor(x);
//  float yFloor = y - std::floor(y);

//  ui32 X = static_cast<ui32>(xFloor) % 256;
//  ui32 Y = static_cast<ui32>(yFloor) % 256;

//  vec2    topLeft(xFloor, yFloor - 1.0f);
//  vec2    topRight(xFloor - 1.0f, yFloor - 1.0f);
//  vec2    bottomLeft(xFloor, yFloor);
//  vec2    bottomRight(xFloor - 1.0f, yFloor);


// }

float   randomNoise(float, float, ui32& seed)
{
    return prn(seed);
}

int repeat = -1;

constexpr int p[] = { 151,160,137,91,90,15,                 // Hash lookup table as defined by Ken Perlin.  This is a randomly
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,    // arranged array of all numbers from 0-255 inclusive.
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
    151,160,137,91,90,15,                   // Hash lookup table as defined by Ken Perlin.  This is a randomly
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,    // arranged array of all numbers from 0-255 inclusive.
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

static int  inc(int num)
{
    num++;
    if (repeat > 0) num %= repeat;

    return num;
}

static float grad(int hash, float x, float y, float z)
{
    int h = hash & 15;                                  // Take the hashed value and take the first 4 bits of it (15 == 0b1111)
    float u = h < 8 /* 0b1000 */ ? x : y;               // If the most significant bit (MSB) of the hash is 0 then set u = x.  Otherwise y.

    float v;                                            // In Ken Perlin's original implementation this was another conditional operator (?:).  I
                                                        // expanded it for readability.

    if(h < 4 /* 0b0100 */)                              // If the first and second significant bits are 0 set v = y
        v = y;
    else if(h == 12 /* 0b1100 */ || h == 14 /* 0b1110*/)// If the first and second significant bits are 1 set v = x
        v = x;
    else                                                // If the first and second significant bits are not equal (0/1, 1/0) set v = z
        v = z;

    return ((h&1) == 0 ? u : -u)+((h&2) == 0 ? v : -v); // Use the last 2 bits to decide if u and v are positive or negative.  Then return their addition.
}

static float fade(float t)
{
                                                        // Fade function as defined by Ken Perlin.  This eases coordinate values
                                                        // so that they will "ease" towards integral values.  This ends up smoothing
                                                        // the final output.
    return t * t * t * (t * (t * 6 - 15) + 10);         // 6t^5 - 15t^4 + 10t^3
}

static float lerp(float a, float b, float x)
{
    return a + x * (b - a);
}

float   perlin(float x, float y, float z)
{
    if(repeat > 0)
    {
        x = std::fmod(x, repeat);
        y = std::fmod(y, repeat);
        z = std::fmod(z, repeat);
    }

    int xi = (int)x & 255;                              // Calculate the "unit cube" that the point asked will be located in
    int yi = (int)y & 255;                              // The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
    int zi = (int)z & 255;                              // plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.
    float xf = x-(int)x;                                // We also fade the location to smooth the result.
    float yf = y-(int)y;
    float zf = z-(int)z;
    float u = fade(xf);
    float v = fade(yf);
    float w = fade(zf);

    int aaa, aba, aab, abb, baa, bba, bab, bbb;
    aaa = p[p[p[    xi ]+    yi ]+    zi ];
    aba = p[p[p[    xi ]+inc(yi)]+    zi ];
    aab = p[p[p[    xi ]+    yi ]+inc(zi)];
    abb = p[p[p[    xi ]+inc(yi)]+inc(zi)];
    baa = p[p[p[inc(xi)]+    yi ]+    zi ];
    bba = p[p[p[inc(xi)]+inc(yi)]+    zi ];
    bab = p[p[p[inc(xi)]+    yi ]+inc(zi)];
    bbb = p[p[p[inc(xi)]+inc(yi)]+inc(zi)];

    float x1, x2, y1, y2;
    x1 = lerp(  grad (aaa, xf  , yf  , zf),             // The gradient function calculates the dot product between a pseudorandom
                grad (baa, xf-1, yf  , zf),             // gradient vector and the vector from the input coordinate to the 8
                u);                                     // surrounding points in its unit cube.
    x2 = lerp(  grad (aba, xf  , yf-1, zf),             // This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
                grad (bba, xf-1, yf-1, zf),             // values we made earlier.
                u);
    y1 = lerp(x1, x2, v);

    x1 = lerp(  grad (aab, xf  , yf  , zf-1),
                grad (bab, xf-1, yf  , zf-1),
                u);
    x2 = lerp(  grad (abb, xf  , yf-1, zf-1),
                grad (bbb, xf-1, yf-1, zf-1),
                u);
    y2 = lerp (x1, x2, v);

    return (lerp (y1, y2, w)+1)/2;                      // For convenience we bound it to 0 - 1 (theoretical min/max before is -1 - 1)
}

float   octavePerlin(float x, float y, float z, int octaves, float persistence)
{
    float   total = 0;
    float   frequency = 1;
    float   amplitude = 1;
    float   maxValue = 0;           // Used for normalizing result to 0.0 - 1.0
    for(int i = 0; i < octaves; i++)
    {
        total += perlin(x * frequency, y * frequency, z * frequency) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= 2;
    }

    return total/maxValue;
}


// end perlin stufff

std::array<vec3,VERTEX_PER_VOXEL> getVertexRelative( vec3 const& relativeOrigin, vec3ui const& dimension ) {
	std::array<vec3,VERTEX_PER_VOXEL> voxelVertexes;
	for (uint32_t i=0; i<VERTEX_PER_VOXEL; i++) {
		voxelVertexes[i].x = (VOXEL_VERTEXES[i].x + 0.5f) * dimension.x + relativeOrigin.x;
		voxelVertexes[i].y = (VOXEL_VERTEXES[i].y + 0.5f) * dimension.y + relativeOrigin.y;
		voxelVertexes[i].z = (VOXEL_VERTEXES[i].z + 0.5f) * dimension.z + relativeOrigin.z;
	}
	return voxelVertexes;
}


VoxelWorld::WorldIterator& VoxelWorld::WorldIterator::operator++( void ) {
	this->pos3D.x++;
	if (this->pos3D.x == this->limits.x) {
		this->pos3D.y++;
		if (this->pos3D.y == this->limits.y) {
			this->pos3D.z++;
			if (this->pos3D.z < this->limits.z) {
				this->pos3D.x = 0U;
				this->pos3D.y = 0U;
			}	// else pos3D = limits == end
		} else
			this->pos3D.x = 0U;
	}
	return *this;
}

bool VoxelWorld::WorldIterator::operator!=( VoxelWorld::WorldIterator const& other ) const {
	return this->pos3D != other.pos3D;
}


// floor on the ground, two 'towers' of voxels, left and right
VoxelWorld VoxelWorld::voxelGenerator1( vec3ui const& worldLimit ) {
	VoxelWorld	newWorld(worldLimit);
	vec3ui		index(0U);
	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			newWorld[index] = true;
	}
	// left tower, 4 voxel base
	for(index.z=1; index.z<worldLimit.z; index.z++) {
		newWorld[vec3ui{1, worldLimit.y - 2, index.z}] = true;
		newWorld[vec3ui{2, worldLimit.y - 2, index.z}] = true;
		newWorld[vec3ui{1, worldLimit.y - 1, index.z}] = true;
		newWorld[vec3ui{2, worldLimit.y - 1, index.z}] = true;
	}
	// right tower, 4 voxel base
	for(index.z=1; index.z<worldLimit.z; index.z++) {
		newWorld[vec3ui{5, worldLimit.y - 2, index.z}] = true;
		newWorld[vec3ui{6, worldLimit.y - 2, index.z}] = true;
		newWorld[vec3ui{5, worldLimit.y - 1, index.z}] = true;
		newWorld[vec3ui{6, worldLimit.y - 1, index.z}] = true;
	}

	return newWorld;
}

// floor on the ground, rest is random
VoxelWorld VoxelWorld::voxelGenerator2( vec3ui const& worldLimit ) {
	VoxelWorld	newWorld(worldLimit);
	vec3ui		index(0U);

	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			newWorld[index] = true;
	}
	index.x = 0; index.y = 0;
	for(index.z=1; index.z<worldLimit.z; index.z++) {
		for(index.y=0; index.y<worldLimit.y; index.y++) {
			for(index.x=0; index.x<worldLimit.x; index.x++)
				newWorld[index] = (ve::randomFloat() > 0.85f) ? true : false;
		}
	}

	return newWorld;
}

// basic random generation
VoxelWorld VoxelWorld::voxelGenerator3( vec3ui const& worldLimit ) {
	VoxelWorld	newWorld(worldLimit);
	vec3ui		index(0U);

	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			newWorld[index] = true;
	}
	// set ceiling
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			newWorld[vec3ui{index.x, index.y, worldLimit.z - 1}] = true;
	}
	// four columns on corner
	for(index.z=0; index.z<worldLimit.z; index.z++) {
		newWorld[vec3ui{0, 0, index.z}] = true;
		newWorld[vec3ui{worldLimit.x - 1, 0, index.z}] = true;
		newWorld[vec3ui{0, worldLimit.y - 1, index.z}] = true;
		newWorld[vec3ui{worldLimit.x - 1, worldLimit.y - 1, index.z}] = true;
	}
	// random cubes, with penality along z (the higher the more difficult the spawn)
	for(index.z=1; index.z<worldLimit.z - 1; index.z++) {
		float t = static_cast<float>(index.z) / worldLimit.z;
		float factor = 1.0f - 1.0f * t;
		for(index.y=1; index.y<worldLimit.y-1; index.y++) {
			for(index.x=1; index.x<worldLimit.x-1; index.x++)
				newWorld[index] = ((ve::randomFloat() * factor) > 0.3f) ? true : false;
		}
	}

	return newWorld;
}

// four voxels in the middle
VoxelWorld VoxelWorld::voxelGenerator4( vec3ui const& worldLimit ) {
	VoxelWorld newWorld(worldLimit);

	newWorld[vec3ui{(worldLimit.x - 1) / 2, (worldLimit.y - 1) / 2, (worldLimit.z - 1) / 2}] = true;
	newWorld[vec3ui{(worldLimit.x - 1) / 2 + 1, (worldLimit.y - 1) / 2, (worldLimit.z - 1) / 2}] = true;
	newWorld[vec3ui{(worldLimit.x - 1) / 2, (worldLimit.y - 1) / 2, (worldLimit.z - 1) / 2 + 1}] = true;
	newWorld[vec3ui{(worldLimit.x - 1) / 2 + 1, (worldLimit.y - 1) / 2, (worldLimit.z - 1) / 2 + 1}] = true;
	return newWorld;
}

// sequence of rectangluar prisms, smaller in area going to the top
VoxelWorld VoxelWorld::voxelGenerator5( vec3ui const& worldLimit ) {
	VoxelWorld	newWorld(worldLimit);
	vec3ui		index(0U);
	uint32_t	hBlock = 0U;

	while (index.z < worldLimit.z) {
		for(; index.z<hBlock+2; index.z++) {
			for(index.y=hBlock; index.y<worldLimit.y - hBlock; index.y++) {
				for(index.x=hBlock; index.x<worldLimit.x - hBlock; index.x++)
					newWorld[index] = true;
			}
		}
		hBlock += 2;
	}
	return newWorld;
}

// 4 towers at the corners with different height 
VoxelWorld VoxelWorld::voxelGenerator6( vec3ui const& worldLimit ) {
	VoxelWorld	newWorld(worldLimit);
	vec3ui		index(0U);
	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			newWorld[index] = true;
	}
	newWorld[vec3ui{0, 0, 1}] = true;
	newWorld[vec3ui{0, 0, 2}] = true;
	newWorld[vec3ui{0, 0, 3}] = true;

	newWorld[vec3ui{worldLimit.x - 1, 0, 1}] = true;
	newWorld[vec3ui{worldLimit.x - 1, 0, 2}] = true;
	newWorld[vec3ui{worldLimit.x - 1, 0, 3}] = true;
	newWorld[vec3ui{worldLimit.x - 1, 0, 4}] = true;

	newWorld[vec3ui{0, worldLimit.y - 1, 1}] = true;
	newWorld[vec3ui{0, worldLimit.y - 1, 2}] = true;
	newWorld[vec3ui{0, worldLimit.y - 1, 3}] = true;
	newWorld[vec3ui{0, worldLimit.y - 1, 4}] = true;
	newWorld[vec3ui{0, worldLimit.y - 1, 5}] = true;

	newWorld[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 1}] = true;
	newWorld[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 2}] = true;
	newWorld[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 3}] = true;
	newWorld[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 4}] = true;
	newWorld[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 5}] = true;
	newWorld[vec3ui{worldLimit.x - 1, worldLimit.y - 1, 6}] = true;
	return newWorld;
}

// empty box
VoxelWorld VoxelWorld::voxelGenerator7( vec3ui const& worldLimit ) {
	VoxelWorld	newWorld(worldLimit);
	vec3ui		index(0U);

	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++) {
			if (index.y % 2 and index.x % 2)
				continue;
			newWorld[index] = true;
		}
	}
	// set ceiling
	index.z = worldLimit.z - 1;
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++) {
			if (index.y % 2 or index.x % 2)
				continue;
			newWorld[index] = true;
		}
	}
	// set left face
	index.x = 0;
	for(index.z=0; index.z<worldLimit.z; index.z++) {
		for(index.y=0; index.y<worldLimit.y; index.y++)
		newWorld[index] = true;
	}
	// set right face
	index.x = worldLimit.x - 1;
	for(index.z=0; index.z<worldLimit.z; index.z++) {
		for(index.y=0; index.y<worldLimit.y; index.y++)
		newWorld[index] = true;
	}
	return newWorld;
}

// floor on the ground
VoxelWorld VoxelWorld::voxelGenerator8( vec3ui const& worldLimit ) {
	VoxelWorld	newWorld(worldLimit);
	vec3ui		index{0U, 0U, 0U};
	// set floor
	for(index.y=0; index.y<worldLimit.y; index.y++) {
		for(index.x=0; index.x<worldLimit.x; index.x++)
			newWorld[index] = true;
	}
	return newWorld;
}

VoxelWorld::VoxelWorld( vec3ui const& size ) {
	this->size = size;
	this->world = std::vector<bool>(this->size.x * this->size.y * this->size.z, false);
}

std::vector<bool>::reference VoxelWorld::operator[]( vec3ui const& pos ) {
	if ((pos.x >= this->size.x) or
		(pos.y >= this->size.y) or
		(pos.z >= this->size.z))
			throw std::runtime_error("Voxel position out of world");

	return this->world[pos.x + pos.y * this->size.x + pos.z * this->size.x * this->size.y];
}

std::vector<bool>::const_reference VoxelWorld::operator[]( vec3ui const& pos ) const {
	if ((pos.x >= this->size.x) or
		(pos.y >= this->size.y) or
		(pos.z >= this->size.z))
			throw std::runtime_error("Voxel position out of world");

	return this->world[pos.x + pos.y * this->size.x + pos.z * this->size.x * this->size.y];
}

bool VoxelWorld::isVoxel( vec3ui const& pos ) const {
	return (*this)[pos];
}

void VoxelWorld::setVoxel( vec3ui const& pos, bool value ) {
	(*this)[pos] = value;
}

void VoxelWorld::setVoxel( vec3ui const& start, vec3ui const& end, bool value ) {
	if ((start.x >= this->size.x) or
		(start.y >= this->size.y) or
		(start.z >= this->size.z) or
		(end.x > this->size.x) or
		(end.y > this->size.y) or
		(end.z > this->size.z))
			throw std::runtime_error("Voxel position(s) out of world");

	vec3ui index = start;
	for (; index.z < end.z; index.z++) {
		for (index.y=start.y; index.y < end.y; index.y++) {
			for (index.x=start.x; index.x < end.x; index.x++)
				(*this)[index] = value;
		}
	}

}

vec3ui VoxelWorld::getBoxelSize( vec3ui const& startVoxel ) const noexcept {
	if (this->isVoxel(startVoxel) == false)
		return vec3ui(0U);

	vec3ui start(startVoxel), next(startVoxel), boxelSize(1U);
	// find longest line of consecutive voxels
	for (next.x = start.x + 1; next.x < this->size.x; next.x++) {
		if (this->isVoxel(next) == false)
			break;
		boxelSize.x++;
	}
	// find widest rectangle of voxels
	for (next.y = start.y + 1; next.y < this->size.y; next.y++) {
		for (next.x = start.x; next.x < start.x + boxelSize.x; next.x++) {
			if (this->isVoxel(next) == false)
				break;
		}
		if (next.x < start.x + boxelSize.x) break;
		boxelSize.y++;
	}
	// find biggest rectangular prism of voxels
	for (next.z = start.z + 1; next.z < this->size.z; next.z++) {
		for (next.y = start.y; next.y < start.y + boxelSize.y; next.y++) {
			for (next.x = start.x; next.x < start.x + boxelSize.x; next.x++) {
				if (this->isVoxel(next) == false) break;
			}
			if (next.x < start.x + boxelSize.x) break;
		}
		if ((next.x < start.x + boxelSize.x) or (next.y < start.y + boxelSize.y)) break;
		boxelSize.z++;
	}
	return boxelSize;
}


void WorldGenerator::HistoryWorlds::add(vec2i const& newPos) {
	// drop the oldest position visited if the limit of 
	// the total positions visited is reached
	if (counter.size() == this->max) {
		vec2i const& lastPosInHistory = history.front();
		history.pop_front();

		auto it = counter.find(lastPosInHistory);
		if (--(it->second) == 0)
			counter.erase(it);
		// NB it should also remove the world from GPU and reload model
	}
	history.push_back(newPos);
	++counter[newPos];
}

bool WorldGenerator::HistoryWorlds::hasVisited(vec2i const& pos) const {
	return this->counter.find(pos) != this->counter.end();
}

void WorldGenerator::init( void ) {
	this->builder.emptyData();
	this->addeNewWorld(vec2i(0));
}

bool WorldGenerator::spawnCloseByWorlds( vec3 const& playerPos ) {
	vec2i currentWorldPos{
		static_cast<int32_t>(playerPos.x) / static_cast<int32_t>(this->worldSize.x),
		static_cast<int32_t>(playerPos.y) / static_cast<int32_t>(this->worldSize.y)
	};
	if (playerPos.x < 0.0f)
		currentWorldPos.x -= 1;
	if (playerPos.y < 0.0f)
		currentWorldPos.y -= 1;

	// add a world, if not existent already, in each of these 9 quadrants
	//  __ __ __
	// |NW|N |NE|
	// |__|__|__|
	// | W| M| E|
	// |__|__|__|
	// |SW|S |SE|
	// |__|__|__|
	//
	bool realoadData = false;
	realoadData |= this->addeNewWorld(currentWorldPos);										// M
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x, currentWorldPos.y + 1});		// N
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x + 1, currentWorldPos.y + 1});	// N-E
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x + 1, currentWorldPos.y});		// E
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x + 1, currentWorldPos.y - 1});	// S-E
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x, currentWorldPos.y - 1});		// S
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x - 1, currentWorldPos.y - 1});	// S-W
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x - 1, currentWorldPos.y});		// W
	realoadData |= this->addeNewWorld(vec2i{currentWorldPos.x - 1, currentWorldPos.y + 1});	// N-W
	return realoadData;
}

bool WorldGenerator::addeNewWorld( vec2i const& worldPos ) {
	bool newWorldAdded = this->history.hasVisited(worldPos) == false;
	if (newWorldAdded) {
		this->history.add(worldPos);
		if (this->mode == MODE_VOXEL_STATIC)
			// every single voxel is loaded directly inside the buffer
			this->fillBufferVoxel(worldPos);
		else if (this->mode == MODE_BOXEL)
			// creates a temporary world of voxels, and then run greedy meshing algo
			// to create boxels (aggregates with less vertexes), takes 2 ~ 3 ms to spawn the world
			this->fillBufferBoxel(worldPos);
		else if (this->mode == MODE_VOXEL_STATIC) {/* for voxels procedurally generated, TBD*/} 
	}
	return newWorldAdded;
}

void WorldGenerator::fillBufferVoxel( vec2i const& worldPos ) {
	vec2 relativeOrigin{
		static_cast<float>(worldPos.x * static_cast<int32_t>(this->worldSize.x)),
		static_cast<float>(worldPos.y * static_cast<int32_t>(this->worldSize.y))
	};
	float scalar = Config::noiseScalar;
	uint32_t seed = Config::seed;
	vec3ui	index(0U);
	// set floor
	for(; index.y<this->worldSize.y; index.y++) {
		for(index.x=0; index.x<this->worldSize.x; index.x++) {
			float noiseValue = perlin((static_cast<float>(index.x) + relativeOrigin.x) * scalar, (static_cast<float>(index.y) + relativeOrigin.y) * scalar, seed);
			noiseValue = (noiseValue + 1.0f) / 2.0f;
			vec3 centerVoxel{
				static_cast<float>(index.x) + relativeOrigin.x,
				static_cast<float>(index.y) + relativeOrigin.y,
				noiseValue * 255
			};
			std::array<vec3,VERTEX_PER_VOXEL> voxelVertexes = getVertexRelative(centerVoxel);
			// check every vertex of the cube/voxel to avoid duplicates
			for (uint32_t index : VOXEL_VERTEX_INDEXES)
				this->builder.addVertex(voxelVertexes[index]);
		}
	}
}

void WorldGenerator::fillBufferBoxel( vec2i const& worldPos ) {
	VoxelWorld newWorld = VoxelWorld::voxelGenerator8(this->worldSize);
	vec2 relativeOrigin{
		static_cast<float>(worldPos.x) * static_cast<float>(this->worldSize.x),
		static_cast<float>(worldPos.y) * static_cast<float>(this->worldSize.y),
	};

	for (auto current3Dpos = newWorld.begin(); current3Dpos != newWorld.end(); ++current3Dpos) {
		if (newWorld.isVoxel(*current3Dpos) == false)
			continue;

		vec3ui start = *current3Dpos;
		// find the size of the boxel starting in current3Dpos
		vec3ui boxelSize = newWorld.getBoxelSize(start);
		// deactivate all the voxels contained in the boxel
		newWorld.setVoxel(start, start + boxelSize, false);
		// add the newly found boxel
		vec3 centerBoxel{
			static_cast<float>(start.x) + relativeOrigin.x,
			static_cast<float>(start.y) + relativeOrigin.y,
			static_cast<float>(start.z)
		};
		std::array<vec3,VERTEX_PER_VOXEL> voxelVertexes = getVertexRelative(centerBoxel, boxelSize);
		// check every vertex of the cube/voxel to avoid duplicates
		for (uint32_t index : VOXEL_VERTEX_INDEXES)
			this->builder.addVertex(voxelVertexes[index]);
	}
}

}
