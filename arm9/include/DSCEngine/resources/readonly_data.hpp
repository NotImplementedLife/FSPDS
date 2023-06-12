#pragma once

namespace DSC
{
	/// ReadOnlyData denotes a type of static resource 
	/// that lies either in RAM, FAT or NITRO.
	/// 
	/// This class represents an abstract interface for resource management 
	/// and should be able to retrieve any bytes buffer regardless of its 
	/// localization.
	///
	/// This class can also be derived to describe more specialized resource
	/// formats.
	///
	/// This class is NOT the binary data itself, but only an expression of it.
	/// It is more like a header that resides only in WRAM, whilst the data it
	/// points to can be anywhere.
	///
	/// Instances of this class should be constants, not allow moving/making copies of them.
	/// In fact, only DSC specialized tools have to create reliable ReadOnlyData instances.
	///
	/// Referencing instances of this class should only be done through pointers
	/// (attention to down/upcasts, no polymorphism is being used)
	///
	///
	/// [ !!! POSSIBLE BUG SOURCE !!! ] There is no "Read-Only Memory" thing on DS homebrew. 
	/// All the constants are being placed in EWRAM (4MB). The compiler tries its best to
	/// prevent programmers from altering constant values, but one can simply break the rule
	/// by overwriting a constant only using its dereferenced pointer. Assuming the programmer doesn't
	/// try anything strange, there is still a chance that an undefined behavior/RAM corruption/
	/// some accidental buffer overflow (eg. for i=0;i<10;i--) may overwrite in-memory "read-only" data.
	/// We should keep a careful eye on that and consider it as a potential explanation for any visual glitches
	/// or misterious data corruption. Therefore, when constant data changes its bytes, then something's really messed up.
	
	class ReadOnlyData
	{		
	public:                      /// Some technical observations to account for during implementation:		
		__attribute__ ((packed))
		short header_size;       /// equilavent of sizeof(*this), but foresees class inheritance
		__attribute__ ((packed))
		int data_length;         /// size in bytes of the actual data	
		__attribute__ ((packed))
		char* data_source;       /// if is_file==true, data_source is the file path (fat://..., nitro://...)
		                         /// if is_file==false, data_source is a pointer to WRAM
						         /// if data_source==NULL, then the actual data resides right after *this
		__attribute__ ((packed)) 
		unsigned short flags;    /// bits 0-7  : see ROD_*	
								 /// bits 8-15 : defined by derived formats specifications
								 
		ReadOnlyData(int header_size = sizeof(ReadOnlyData));
		
		/*! \brief writes all binary data to the given address
			\param destination address to extract data to
		 */
		void extract(void* destination) const; 
		
		/*! \brief writes binary data sequence to the given address
			\param destination address to extract data to
			\param offset position in effetive data where copying starts
			\param length length of data to copy
		 */
		void extract(void* destination, int offset, int length) const;
		
		// prevent altering this object
		ReadOnlyData(const ReadOnlyData&) = delete;
		ReadOnlyData(ReadOnlyData&&) = delete;
		ReadOnlyData operator = (const ReadOnlyData&) = delete;
		ReadOnlyData operator = (ReadOnlyData&&) = delete;		
		
		/*! \brief checks if data is written to file
			\return true if effective data is in file, false if in WRAM
		 */
		bool is_file() const;
		
		///*! \brief checks if 
		bool is_compressed() const;
		int get_type() const;
		
		static const int ROD_IS_FILE;       /// tell if the data is in RAM or on a filesystem
		static const int ROD_IS_COMPRESSED; /// 0 = no compression, 1=LZ7 or sth, ...
		static const int ROD_TYPE;
		
		static const int ROD_TYPE_UNKNOWN;
		static const int ROD_TYPE_ASSET;
		static const int ROD_TYPE_FONT;		
		
	};	
}