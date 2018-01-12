#ifndef BITMAP_IMAGE_H
#define BITMAP_IMAGE_H

#include "Common.h"
#include "bitmap_file_header.h"
#include "bitmap_information_header.h"
#include "rgb.h"

struct bitmap_image {

	/* Enum */
	enum channel_mode { rgb_mode = 0, bgr_mode = 1 };
	enum color_plane { blue_plane = 0, green_plane = 1, red_plane = 2 };

	/* Variables */
	string       file_name_;
	unsigned int width_;
	unsigned int height_;
	unsigned int row_increment_;
	unsigned int bytes_per_pixel_;
	channel_mode channel_mode_;
	vector<unsigned char> data_;

	/* Constructors */
	bitmap_image() {
		file_name_ = "";
		width_ = 0;
		height_ = 0;
		row_increment_ = 0;
		bytes_per_pixel_ = 0;
		channel_mode_ = bgr_mode;
	}

	bitmap_image(const string& filename) {
		file_name_ = filename;
		width_ = 0;
		height_ = 0;
		row_increment_ = 0;
		bytes_per_pixel_ = 0;
		channel_mode_ = bgr_mode;
		load_bitmap();
	}

	bitmap_image(const unsigned int width, const unsigned int height) {
		file_name_ = "";
		width_ = width;
		height_ = height;
		row_increment_ = 0;
		bytes_per_pixel_ = 3;
		channel_mode_ = bgr_mode;
		create_bitmap();
	}

	/* Operators */
	inline bool operator!() {
		return (data_.size() == 0 || width_ == 0 || height_ == 0);
	}

	/* Getter functions */
	inline string file_name() const { return file_name_; }
	inline unsigned int width() const { return width_; }
	inline unsigned int height() const { return height_; }
	inline unsigned int bytes_per_pixel() const { return bytes_per_pixel_; }
	inline unsigned int pixel_count() const { return width_ *  height_; }

	/* Functions */
	inline void print_info() {
		printf("\nwidth: %d", width_);
		printf("\nheight: %d", height_);
		printf("\nrow_increment_: %d", row_increment_);
		printf("\nchannel_mode_: %d", channel_mode_);
		printf("\nbytes_per_pixel_: %d", bytes_per_pixel_);
		//printf("\ndata_: ");
		//print_data();

	}

	/* Print all the image data */
	inline void print_data() {
		int size = width_ * height_ *bytes_per_pixel_;
		for (int i = 0; i < size; i++){
			printf("%u ", data_[i]);
		}
	}
	
	unsigned char * data_vector_to_array()
	{
		int total_size = data_.size();
		unsigned char* arr_data = new unsigned char[total_size];
		
		for (int i = 0; i < total_size; ++i){
			data_[i] = arr_data[i];
		}
		return arr_data;
	}


	inline void create_bitmap() 
	{
		row_increment_ = width_ * bytes_per_pixel_;
		data_.resize(height_ * row_increment_);
	}

	inline unsigned char* row(unsigned int row_index) const 
	{
		return const_cast<unsigned char*>(&data_[(row_index * row_increment_)]);
	}

	inline size_t file_size(const string& file_name) const {
		ifstream file(file_name.c_str(), ios::in | ios::binary);
		if (!file) return 0;
		file.seekg(0, ios::end);
		return static_cast<size_t>(file.tellg());
	}

	inline void get_pixel(const unsigned int x, const unsigned int y, unsigned char& red,
		unsigned char& green, unsigned char& blue) const {
		const unsigned int y_offset = y * row_increment_;
		const unsigned int x_offset = x * bytes_per_pixel_;

		blue = data_[y_offset + x_offset + 0];
		green = data_[y_offset + x_offset + 1];
		red = data_[y_offset + x_offset + 2];
	}

	template <typename RGB>
	inline void get_pixel(const unsigned int x, const unsigned int y, RGB& colour) const {
		get_pixel(x, y, colour.red, colour.green, colour.blue);
	}

	inline void set_pixel(const unsigned int x, const unsigned int y, const unsigned char red,
		const unsigned char green, const unsigned char blue) {
		const unsigned int y_offset = y * row_increment_;
		const unsigned int x_offset = x * bytes_per_pixel_;

		data_[y_offset + x_offset + 0] = blue;
		data_[y_offset + x_offset + 1] = green;
		data_[y_offset + x_offset + 2] = red;
	}

	template <typename RGB>
	inline void set_pixel(const unsigned int x, const unsigned int y, const RGB& colour) {
		set_pixel(x, y, colour.red, colour.green, colour.blue);
	}

	void load_bitmap() {

		ifstream stream(file_name_.c_str(), ios::binary);
		if (!stream) {
			cerr << "ERROR: bitmap_image - file " << file_name_ << " not found!" << endl;
			return;
		}

		bitmap_file_header bfh;
		bitmap_information_header bih;

		bfh.clear();
		bih.clear();

		read_bfh(stream, bfh);
		read_bih(stream, bih);

		if (bfh.type != 19778) {
			bfh.clear();
			bih.clear();

			stream.close();
			cerr << "ERROR: bitmap_image - Invalid type value " << bfh.type << " expected 19778." << endl;
			return;
		}

		if (bih.bit_count != 24) {
			bfh.clear();
			bih.clear();

			stream.close();
			cerr << "ERROR: bitmap_image - Invalid bit depth " << bih.bit_count << " expected 24." << endl;
			return;
		}

		if (bih.size != bih.struct_size()) {
			bfh.clear();
			bih.clear();

			stream.close();
			cerr << "ERROR: bitmap_image - Invalid BIH size " << bih.size << " expected " << bih.struct_size() << endl;
			return;
		}

		width_ = bih.width;
		height_ = bih.height;

		bytes_per_pixel_ = bih.bit_count >> 3;

		unsigned int padding = (4 - ((3 * width_) % 4)) % 4;
		char padding_data[4] = { 0, 0, 0, 0 };

		size_t bitmap_file_size = file_size(file_name_);

		size_t bitmap_logical_size = (height_ * width_ * bytes_per_pixel_) +
			(height_ * padding) +
			bih.struct_size() +
			bfh.struct_size();

		if (bitmap_file_size != bitmap_logical_size) {
			bfh.clear();
			bih.clear();

			stream.close();
			cerr << "ERROR: bitmap_image - Mismatch between logical and physical sizes of bitmap. " <<
				"Logical: " << bitmap_logical_size << " " <<
				"Physical: " << bitmap_file_size << endl;
			return;
		}

		create_bitmap();

		for (unsigned int i = 0; i < height_; ++i) {
			unsigned char* data_ptr = row(height_ - i - 1); // read in inverted row order
			stream.read(reinterpret_cast<char*>(data_ptr), sizeof(char)* bytes_per_pixel_ * width_);
			stream.read(padding_data, padding);
		}
	}

	void save_image(const string& file_name) const {
		ofstream stream(file_name.c_str(), ios::binary);

		if (!stream) {
			cerr << "bitmap_image::save_image(): Error - Could not open file " << file_name << " for writing!" << endl;
			return;
		}

		bitmap_information_header bih;

		bih.width = width_;
		bih.height = height_;
		bih.bit_count = static_cast<unsigned short>(bytes_per_pixel_ << 3);
		bih.clr_important = 0;
		bih.clr_used = 0;
		bih.compression = 0;
		bih.planes = 1;
		bih.size = bih.struct_size();
		bih.x_pels_per_meter = 0;
		bih.y_pels_per_meter = 0;
		bih.size_image = (((bih.width * bytes_per_pixel_) + 3) & 0x0000FFFC) * bih.height;

		bitmap_file_header bfh;

		bfh.type = 19778;
		bfh.size = bfh.struct_size() + bih.struct_size() + bih.size_image;
		bfh.reserved1 = 0;
		bfh.reserved2 = 0;
		bfh.off_bits = bih.struct_size() + bfh.struct_size();

		write_bfh(stream, bfh);
		write_bih(stream, bih);

		unsigned int padding = (4 - ((3 * width_) % 4)) % 4;
		char padding_data[4] = { 0x00, 0x00, 0x00, 0x00 };

		for (unsigned int i = 0; i < height_; ++i) {
			const unsigned char* data_ptr = &data_[(row_increment_ * (height_ - i - 1))];
			stream.write(reinterpret_cast<const char*>(data_ptr), sizeof(unsigned char)* bytes_per_pixel_ * width_);
			stream.write(padding_data, padding);
		}

		stream.close();
	}

	inline bool big_endian() const {
		unsigned int v = 0x01;
		return (1 != reinterpret_cast<char*>(&v)[0]);
	}

	inline unsigned short flip(const unsigned short& v) const {
		return ((v >> 8) | (v << 8));
	}

	template <typename T>
	inline void write_to_stream(ofstream& stream, const T& t) const {
		stream.write(reinterpret_cast<const char*>(&t), sizeof(T));
	}

	template <typename T>
	inline void read_from_stream(ifstream& stream, T& t) {
		stream.read(reinterpret_cast<char*>(&t), sizeof(T));
	}

	inline void read_bfh(ifstream& stream, bitmap_file_header& bfh) {
		read_from_stream(stream, bfh.type);
		read_from_stream(stream, bfh.size);
		read_from_stream(stream, bfh.reserved1);
		read_from_stream(stream, bfh.reserved2);
		read_from_stream(stream, bfh.off_bits);

		if (big_endian()) {
			bfh.type = flip(bfh.type);
			bfh.size = flip(bfh.size);
			bfh.reserved1 = flip(bfh.reserved1);
			bfh.reserved2 = flip(bfh.reserved2);
			bfh.off_bits = flip(bfh.off_bits);
		}
	}

	inline void write_bfh(ofstream& stream, const bitmap_file_header& bfh) const {
		if (big_endian()) {
			write_to_stream(stream, flip(bfh.type));
			write_to_stream(stream, flip(bfh.size));
			write_to_stream(stream, flip(bfh.reserved1));
			write_to_stream(stream, flip(bfh.reserved2));
			write_to_stream(stream, flip(bfh.off_bits));
		}
		else {
			write_to_stream(stream, bfh.type);
			write_to_stream(stream, bfh.size);
			write_to_stream(stream, bfh.reserved1);
			write_to_stream(stream, bfh.reserved2);
			write_to_stream(stream, bfh.off_bits);
		}
	}

	inline void read_bih(ifstream& stream, bitmap_information_header& bih) {
		read_from_stream(stream, bih.size);
		read_from_stream(stream, bih.width);
		read_from_stream(stream, bih.height);
		read_from_stream(stream, bih.planes);
		read_from_stream(stream, bih.bit_count);
		read_from_stream(stream, bih.compression);
		read_from_stream(stream, bih.size_image);
		read_from_stream(stream, bih.x_pels_per_meter);
		read_from_stream(stream, bih.y_pels_per_meter);
		read_from_stream(stream, bih.clr_used);
		read_from_stream(stream, bih.clr_important);

		if (big_endian()) {
			bih.size = flip(bih.size);
			bih.width = flip(bih.width);
			bih.height = flip(bih.height);
			bih.planes = flip(bih.planes);
			bih.bit_count = flip(bih.bit_count);
			bih.compression = flip(bih.compression);
			bih.size_image = flip(bih.size_image);
			bih.x_pels_per_meter = flip(bih.x_pels_per_meter);
			bih.y_pels_per_meter = flip(bih.y_pels_per_meter);
			bih.clr_used = flip(bih.clr_used);
			bih.clr_important = flip(bih.clr_important);
		}
	}


	inline void write_bih(ofstream& stream, const bitmap_information_header& bih) const {
		if (big_endian()) {
			write_to_stream(stream, flip(bih.size));
			write_to_stream(stream, flip(bih.width));
			write_to_stream(stream, flip(bih.height));
			write_to_stream(stream, flip(bih.planes));
			write_to_stream(stream, flip(bih.bit_count));
			write_to_stream(stream, flip(bih.compression));
			write_to_stream(stream, flip(bih.size_image));
			write_to_stream(stream, flip(bih.x_pels_per_meter));
			write_to_stream(stream, flip(bih.y_pels_per_meter));
			write_to_stream(stream, flip(bih.clr_used));
			write_to_stream(stream, flip(bih.clr_important));
		}
		else {
			write_to_stream(stream, bih.size);
			write_to_stream(stream, bih.width);
			write_to_stream(stream, bih.height);
			write_to_stream(stream, bih.planes);
			write_to_stream(stream, bih.bit_count);
			write_to_stream(stream, bih.compression);
			write_to_stream(stream, bih.size_image);
			write_to_stream(stream, bih.x_pels_per_meter);
			write_to_stream(stream, bih.y_pels_per_meter);
			write_to_stream(stream, bih.clr_used);
			write_to_stream(stream, bih.clr_important);
		}
	}

};


#endif
