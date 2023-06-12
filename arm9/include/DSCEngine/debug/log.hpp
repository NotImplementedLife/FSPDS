/*!
 * \file log.hpp
 * \brief send debug messages to the emulator
 */

#pragma once

namespace DSC::Debug
{		
	/*!
	 * \brief Sends a \b log message to the emulator
	 * \param [in] message log message, allows printf-like wildcards
	 * \param [in] ... wildcard arguments
	 *
	 * \details Allowed wildcards:
	 * - <code>\%i</code> : signed integer	 
	 * - <code>\%u</code> : unsigned integer	 
	 * - <code>\%s</code> : character string
	 * - <code>\%b</code> : short boolean (T/F)
	 * - <code>\%B</code> : long boolean (True/False)
	 * - <code>\%x</code> : lowercase hex (123ab)
	 * - <code>\%X</code> : uppercase hex (123AB)
	 * 
	 * Usage:
	 * \code{.cpp} int x=2; 
	 *  DSC::Debug::log("My int = %i", x)
	 * \endcode
	*/
	void log(const char* message, ...);

	/*!
	 * \brief Sends a \b warning message to the emulator
	 * \param [in] message log message, allows printf-like wildcards
	 * \param [in] ... wildcard arguments
	 * 
	 * See DSC::Debug::log() for more details
	*/
	void warn(const char* message, ...);
	
	
	/*!
	 * \brief Sends an \b error message to the emulator
	 * \param [in] message log message, allows printf-like wildcards
	 * \param [in] ... wildcard arguments
	 * 
	 * See DSC::Debug::log() for more details
	*/
	void error(const char* message, ...);
}