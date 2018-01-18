/* To Layer L = 1 -> there is 1089  * 3 values (3 by coordinate)
To Layer L = {2,3,4, ...} -> there is 864 * 3 values

There is 2048 triangles into layer L = 1
There is 1536 triangles into layers L = {2,3,4,..}
*/

#ifndef CLIPMAP_H
#define CLIPMAP_H

#include "Common.h"
#include "PerlinNoise.h"

#define N_COORD 3

#define N_1 33									// num of vertex to the first clipmap
//#define N_2 864									// num of vertex to the remain clipmaps (864)

#define N_TRIANG 32

#define UP_TRIANG 8
#define BOTTOM_TRIANG 8
#define LEFT_TRIANG 8
#define RIGHT_TRIANG 8

#define SIDE 16

#define TOTAL_TRIANG_1 2048
#define TOT_TRIANG_2 1536

struct Clipmap {

	GLfloat* coordinates;						// these coordinates will be updated
	GLfloat* heights;							// contains heights generate by Perlin Noise
	GLfloat* triangulationCoord;				// contains coordinates to draw (passed to buffer)
	int nLayer;									// 1: first clipmap    0: other clipmap
	int size;

	/* Constructors */
	Clipmap(){}

	void initialize(int numLayer)
	{
		coordinates = new GLfloat[N_COORD * N_1 * N_1];					// 3267
		heights = new GLfloat[N_1 * N_1];
		triangulationCoord = new GLfloat[N_COORD * N_COORD * N_TRIANG * 2 * N_TRIANG];			// 18432
		nLayer = numLayer;
		size = N_COORD * N_1 * N_1;
	};

	/* Destructor */
	~Clipmap()
	{
		delete coordinates;
		delete heights;
		delete triangulationCoord;
	}


	/* Functions */
	void readCoordinates()
	{
		ifstream file;

		switch (nLayer) {
		case 1:
			file.open("clipmap_data/triangulization_1.txt");
			break;
		case 2:
			file.open("clipmap_data/triangulization_2.txt");
			break;
		case 3:
			file.open("clipmap_data/triangulization_3.txt");
			break;
		case 4:
			file.open("clipmap_data/triangulization_4.txt");
			break;
		case 5:
			file.open("clipmap_data/triangulization_5.txt");
			break;
		case 6:
			file.open("clipmap_data/triangulization_6.txt");
			break;
		case 7:
			file.open("clipmap_data/triangulization_7.txt");
			break;

		default:
			break;
		}

		for (int i = 0; i < size; i += 3) {
			file >> coordinates[i];
			file >> coordinates[i + 1];
			file >> coordinates[i + 2];
		}
	}

	/* Generate heights with Perlin Noise */
	void generateHeigths()
	{
		PerlinNoise perlin;
		int total_heigts = size / N_COORD;
		for (int i = 0; i < total_heigts; i++) {
			heights[i] = perlin.noise(coordinates[N_COORD * i], coordinates[N_COORD * i + 1], coordinates[N_COORD * i + 2]);
		}

	}

	/* The 'y' coordinates will be updated by heights generate by PerlinNoise */
	void perlinCoordinates()
	{
		int total_heigts = size / N_COORD;
		for (int i = 0; i < total_heigts; i++) {
			coordinates[N_COORD * i + 1] = heights[i];
		}
	}

	/* Some 'y' coordinates will be updated to delete the holes to L = {2,3,4,..} */
	void deleteHole() 
	{
		/* top & bottom */
		int uppOff = 4;
		int leftOff = 3 * 33 + 1;
		int right = 3 * 33 * 2 - 2;
		int bottomOff = 33 * 3 * 32 + 4;
		int sideOff = 99 * 2;

		for (int i = 0; i < 16; i++) {
			coordinates[uppOff] = (coordinates[uppOff - 3] + coordinates[uppOff + 3]) / 2;
			coordinates[leftOff] = (coordinates[leftOff - 99] + coordinates[leftOff + 99]) / 2;
			coordinates[right] = (coordinates[right - 99] + coordinates[right + 99]) / 2;
			coordinates[bottomOff] = (coordinates[bottomOff - 3] + coordinates[bottomOff + 3]) / 2;
			uppOff += 6;
			bottomOff += 6;
			leftOff += sideOff;
			right += sideOff;
		}
	}

	/* Generate the triangulation coordinates to the center L = 1 */
	void generateCenterTriangulation() {

		int offset = N_COORD * N_COORD;									// 3 * 3
		int width = N_COORD * N_1;										// 3 * 33
		int firstOff = N_TRIANG * N_COORD * N_COORD;					// 32 * 3 * 3
		int secondOff = N_TRIANG * N_COORD * N_COORD * 2;				// 32 * 3 * 3 * 2
		int coordOff = N_1 * N_COORD;									// 33 * 3
		int totalOffset = 0;
		int totalCoordOff = 0;

		for (int k = 0; k < N_TRIANG; k++) {
			for (int i = 0; i < N_TRIANG; i++) {
				triangulationCoord[totalOffset + offset * i + 0] = coordinates[totalCoordOff + N_COORD * i + 0];
				triangulationCoord[totalOffset + offset * i + 1] = coordinates[totalCoordOff + N_COORD * i + 1];
				triangulationCoord[totalOffset + offset * i + 2] = coordinates[totalCoordOff + N_COORD * i + 2];

				triangulationCoord[totalOffset + offset * i + 3] = coordinates[totalCoordOff + N_COORD * i + 3];
				triangulationCoord[totalOffset + offset * i + 4] = coordinates[totalCoordOff + N_COORD * i + 4];
				triangulationCoord[totalOffset + offset * i + 5] = coordinates[totalCoordOff + N_COORD * i + 5];

				triangulationCoord[totalOffset + offset * i + 6] = coordinates[totalCoordOff + width + N_COORD * i + 3];
				triangulationCoord[totalOffset + offset * i + 7] = coordinates[totalCoordOff + width + N_COORD * i + 4];
				triangulationCoord[totalOffset + offset * i + 8] = coordinates[totalCoordOff + width + N_COORD * i + 5];
			}

			for (int i = 0; i < N_TRIANG; i++) {
				triangulationCoord[totalOffset + firstOff + offset * i + 0] = coordinates[totalCoordOff + N_COORD * i + 0];
				triangulationCoord[totalOffset + firstOff + offset * i + 1] = coordinates[totalCoordOff + N_COORD * i + 1];
				triangulationCoord[totalOffset + firstOff + offset * i + 2] = coordinates[totalCoordOff + N_COORD * i + 2];

				triangulationCoord[totalOffset + firstOff + offset * i + 3] = coordinates[totalCoordOff + width + N_COORD * i + 0];
				triangulationCoord[totalOffset + firstOff + offset * i + 4] = coordinates[totalCoordOff + width + N_COORD * i + 1];
				triangulationCoord[totalOffset + firstOff + offset * i + 5] = coordinates[totalCoordOff + width + N_COORD * i + 2];

				triangulationCoord[totalOffset + firstOff + offset * i + 6] = coordinates[totalCoordOff + width + N_COORD * i + 3];
				triangulationCoord[totalOffset + firstOff + offset * i + 7] = coordinates[totalCoordOff + width + N_COORD * i + 4];
				triangulationCoord[totalOffset + firstOff + offset * i + 8] = coordinates[totalCoordOff + width + N_COORD * i + 5];
			}

			totalOffset += secondOff;
			totalCoordOff += coordOff;
		}
	}

	/* Generate the triangulation coordinates to L = {2, 3, ...} */
	void generateSidesTriangulation() {

		/* Top */
		deleteHole();

		int cont = 0;
		int totalOffset = 0, totalCoordOff = 0;
		int offset = N_COORD * N_COORD;									// 3 * 3
		int width = N_COORD * N_1;										// 3 * 33
		int firstOff = N_TRIANG * N_COORD * N_COORD;					// 32 * 3 * 3
		int secondOff = N_TRIANG * N_COORD * N_COORD * 2;				// 32 * 3 * 3 * 2
		int coordOff = N_1 * N_COORD;									// 33 * 3

		for (int k = 0; k < UP_TRIANG; k++) {
			for (int i = 0; i < N_TRIANG; i++) {
				triangulationCoord[totalOffset + offset * i + 0] = coordinates[totalCoordOff + N_COORD * i + 0];
				triangulationCoord[totalOffset + offset * i + 1] = coordinates[totalCoordOff + N_COORD * i + 1];
				triangulationCoord[totalOffset + offset * i + 2] = coordinates[totalCoordOff + N_COORD * i + 2];

				triangulationCoord[totalOffset + offset * i + 3] = coordinates[totalCoordOff + N_COORD * i + 3];
				triangulationCoord[totalOffset + offset * i + 4] = coordinates[totalCoordOff + N_COORD * i + 4];
				triangulationCoord[totalOffset + offset * i + 5] = coordinates[totalCoordOff + N_COORD * i + 5];

				triangulationCoord[totalOffset + offset * i + 6] = coordinates[totalCoordOff + width + N_COORD * i + 3];
				triangulationCoord[totalOffset + offset * i + 7] = coordinates[totalCoordOff + width + N_COORD * i + 4];
				triangulationCoord[totalOffset + offset * i + 8] = coordinates[totalCoordOff + width + N_COORD * i + 5];
			}

			for (int i = 0; i < N_TRIANG; i++) {
				triangulationCoord[totalOffset + firstOff + offset * i + 0] = coordinates[totalCoordOff + N_COORD * i + 0];
				triangulationCoord[totalOffset + firstOff + offset * i + 1] = coordinates[totalCoordOff + N_COORD * i + 1];
				triangulationCoord[totalOffset + firstOff + offset * i + 2] = coordinates[totalCoordOff + N_COORD * i + 2];

				triangulationCoord[totalOffset + firstOff + offset * i + 3] = coordinates[totalCoordOff + width + N_COORD * i + 0];
				triangulationCoord[totalOffset + firstOff + offset * i + 4] = coordinates[totalCoordOff + width + N_COORD * i + 1];
				triangulationCoord[totalOffset + firstOff + offset * i + 5] = coordinates[totalCoordOff + width + N_COORD * i + 2];

				triangulationCoord[totalOffset + firstOff + offset * i + 6] = coordinates[totalCoordOff + width + N_COORD * i + 3];
				triangulationCoord[totalOffset + firstOff + offset * i + 7] = coordinates[totalCoordOff + width + N_COORD * i + 4];
				triangulationCoord[totalOffset + firstOff + offset * i + 8] = coordinates[totalCoordOff + width + N_COORD * i + 5];
			}

			totalOffset += secondOff;				// totalOffset = 4608
			totalCoordOff += coordOff;				// totalCoordOff = 792
		}

		int rightCoordOff = totalCoordOff;

		/* Left */
		firstOff = 8 * N_COORD * N_COORD;					// 32 * 3 * 3
		for (int k = 0; k < SIDE; k++) {
			for (int i = 0; i < LEFT_TRIANG; i++) {
				triangulationCoord[totalOffset + offset * i + 0] = coordinates[totalCoordOff + N_COORD * i + 0];
				triangulationCoord[totalOffset + offset * i + 1] = coordinates[totalCoordOff + N_COORD * i + 1];
				triangulationCoord[totalOffset + offset * i + 2] = coordinates[totalCoordOff + N_COORD * i + 2];

				triangulationCoord[totalOffset + offset * i + 3] = coordinates[totalCoordOff + N_COORD * i + 3];
				triangulationCoord[totalOffset + offset * i + 4] = coordinates[totalCoordOff + N_COORD * i + 4];
				triangulationCoord[totalOffset + offset * i + 5] = coordinates[totalCoordOff + N_COORD * i + 5];

				triangulationCoord[totalOffset + offset * i + 6] = coordinates[totalCoordOff + width + N_COORD * i + 3];
				triangulationCoord[totalOffset + offset * i + 7] = coordinates[totalCoordOff + width + N_COORD * i + 4];
				triangulationCoord[totalOffset + offset * i + 8] = coordinates[totalCoordOff + width + N_COORD * i + 5];
			}

			for (int i = 0; i < LEFT_TRIANG; i++) {
				triangulationCoord[totalOffset + firstOff + offset * i + 0] = coordinates[totalCoordOff + N_COORD * i + 0];
				triangulationCoord[totalOffset + firstOff + offset * i + 1] = coordinates[totalCoordOff + N_COORD * i + 1];
				triangulationCoord[totalOffset + firstOff + offset * i + 2] = coordinates[totalCoordOff + N_COORD * i + 2];

				triangulationCoord[totalOffset + firstOff + offset * i + 3] = coordinates[totalCoordOff + width + N_COORD * i + 0];
				triangulationCoord[totalOffset + firstOff + offset * i + 4] = coordinates[totalCoordOff + width + N_COORD * i + 1];
				triangulationCoord[totalOffset + firstOff + offset * i + 5] = coordinates[totalCoordOff + width + N_COORD * i + 2];

				triangulationCoord[totalOffset + firstOff + offset * i + 6] = coordinates[totalCoordOff + width + N_COORD * i + 3];
				triangulationCoord[totalOffset + firstOff + offset * i + 7] = coordinates[totalCoordOff + width + N_COORD * i + 4];
				triangulationCoord[totalOffset + firstOff + offset * i + 8] = coordinates[totalCoordOff + width + N_COORD * i + 5];
			}

			totalOffset += LEFT_TRIANG * N_COORD * N_COORD * 2;
			totalCoordOff += coordOff;
		}

		/* Right */
		rightCoordOff += 24 * 3;
		totalCoordOff = rightCoordOff;

		for (int k = 0; k < SIDE; k++) {
			for (int i = 0; i < RIGHT_TRIANG; i++) {
				triangulationCoord[totalOffset + offset * i + 0] = coordinates[totalCoordOff + N_COORD * i + 0];
				triangulationCoord[totalOffset + offset * i + 1] = coordinates[totalCoordOff + N_COORD * i + 1];
				triangulationCoord[totalOffset + offset * i + 2] = coordinates[totalCoordOff + N_COORD * i + 2];

				triangulationCoord[totalOffset + offset * i + 3] = coordinates[totalCoordOff + N_COORD * i + 3];
				triangulationCoord[totalOffset + offset * i + 4] = coordinates[totalCoordOff + N_COORD * i + 4];
				triangulationCoord[totalOffset + offset * i + 5] = coordinates[totalCoordOff + N_COORD * i + 5];

				triangulationCoord[totalOffset + offset * i + 6] = coordinates[totalCoordOff + width + N_COORD * i + 3];
				triangulationCoord[totalOffset + offset * i + 7] = coordinates[totalCoordOff + width + N_COORD * i + 4];
				triangulationCoord[totalOffset + offset * i + 8] = coordinates[totalCoordOff + width + N_COORD * i + 5];
			}

			for (int i = 0; i < RIGHT_TRIANG; i++) {
				triangulationCoord[totalOffset + firstOff + offset * i + 0] = coordinates[totalCoordOff + N_COORD * i + 0];
				triangulationCoord[totalOffset + firstOff + offset * i + 1] = coordinates[totalCoordOff + N_COORD * i + 1];
				triangulationCoord[totalOffset + firstOff + offset * i + 2] = coordinates[totalCoordOff + N_COORD * i + 2];

				triangulationCoord[totalOffset + firstOff + offset * i + 3] = coordinates[totalCoordOff + width + N_COORD * i + 0];
				triangulationCoord[totalOffset + firstOff + offset * i + 4] = coordinates[totalCoordOff + width + N_COORD * i + 1];
				triangulationCoord[totalOffset + firstOff + offset * i + 5] = coordinates[totalCoordOff + width + N_COORD * i + 2];

				triangulationCoord[totalOffset + firstOff + offset * i + 6] = coordinates[totalCoordOff + width + N_COORD * i + 3];
				triangulationCoord[totalOffset + firstOff + offset * i + 7] = coordinates[totalCoordOff + width + N_COORD * i + 4];
				triangulationCoord[totalOffset + firstOff + offset * i + 8] = coordinates[totalCoordOff + width + N_COORD * i + 5];
			}
			totalOffset += LEFT_TRIANG * N_COORD * N_COORD * 2;
			totalCoordOff += coordOff;
		}

		/* Bottom */
		firstOff = N_TRIANG * N_COORD * N_COORD;
		totalCoordOff = 2376;

		for (int k = 0; k < BOTTOM_TRIANG; k++) {
			for (int i = 0; i < N_TRIANG; i++) {
				triangulationCoord[totalOffset + offset * i + 0] = coordinates[totalCoordOff + N_COORD * i + 0];
				triangulationCoord[totalOffset + offset * i + 1] = coordinates[totalCoordOff + N_COORD * i + 1];
				triangulationCoord[totalOffset + offset * i + 2] = coordinates[totalCoordOff + N_COORD * i + 2];

				triangulationCoord[totalOffset + offset * i + 3] = coordinates[totalCoordOff + N_COORD * i + 3];
				triangulationCoord[totalOffset + offset * i + 4] = coordinates[totalCoordOff + N_COORD * i + 4];
				triangulationCoord[totalOffset + offset * i + 5] = coordinates[totalCoordOff + N_COORD * i + 5];

				triangulationCoord[totalOffset + offset * i + 6] = coordinates[totalCoordOff + width + N_COORD * i + 3];
				triangulationCoord[totalOffset + offset * i + 7] = coordinates[totalCoordOff + width + N_COORD * i + 4];
				triangulationCoord[totalOffset + offset * i + 8] = coordinates[totalCoordOff + width + N_COORD * i + 5];
			}

			for (int i = 0; i < N_TRIANG; i++) {
				triangulationCoord[totalOffset + firstOff + offset * i + 0] = coordinates[totalCoordOff + N_COORD * i + 0];
				triangulationCoord[totalOffset + firstOff + offset * i + 1] = coordinates[totalCoordOff + N_COORD * i + 1];
				triangulationCoord[totalOffset + firstOff + offset * i + 2] = coordinates[totalCoordOff + N_COORD * i + 2];

				triangulationCoord[totalOffset + firstOff + offset * i + 3] = coordinates[totalCoordOff + width + N_COORD * i + 0];
				triangulationCoord[totalOffset + firstOff + offset * i + 4] = coordinates[totalCoordOff + width + N_COORD * i + 1];
				triangulationCoord[totalOffset + firstOff + offset * i + 5] = coordinates[totalCoordOff + width + N_COORD * i + 2];

				triangulationCoord[totalOffset + firstOff + offset * i + 6] = coordinates[totalCoordOff + width + N_COORD * i + 3];
				triangulationCoord[totalOffset + firstOff + offset * i + 7] = coordinates[totalCoordOff + width + N_COORD * i + 4];
				triangulationCoord[totalOffset + firstOff + offset * i + 8] = coordinates[totalCoordOff + width + N_COORD * i + 5];
			}

			totalOffset += secondOff;
			totalCoordOff += N_1 * N_COORD;
		}

	}

};


#endif