/*
 * Resonon API
 *
 * By using this API, the user agrees to the terms and conditions as stated in the
 * document "Resonon API Terms of Use", located on the Resonon website
 * at: http://www.resonon.com/downloads/Resonon_API_Terms_of_Use.pdf.
 *
 */


#ifndef __GUARD_RESONON_IMAGER_X_H
#define __GUARD_RESONON_IMAGER_X_H
#include "resonon_imager_base.h"
#include <string>

namespace Resonon
{
	class XCameraImplementation;

	/**
	* Interface to <a href=http://www.resonon.com/imagers_pika_nir.html>Pika NIR hyperspectral imagers</a>.
	*/
	class RESONONDLL PikaNIR : public ResononImagerBase
	{
		public:
			PikaNIR();
			~PikaNIR();
			void set_calibration_file(const char* calibration_file);
			void connect();
			void disconnect();
			int get_frame_buffer_size_in_bytes();
			unsigned short* get_frame(unsigned short* buffer);
			void start_frame_grabbing();
			void stop_frame_grabbing();
			void set_framerate(const double frames_per_second);
			void set_window_bounds(int start_band,
								   int end_band,
								   int start_sample,
								   int end_sample);
			int get_band_count();
			int get_sample_count();
			int get_window_start_band();
			int get_window_end_band();
			int get_window_start_sample();
			int get_window_end_sample();
			double get_framerate();
			double get_min_integration_time();
			double get_max_integration_time();
			double get_min_framerate();
			double get_max_framerate();
			void set_integration_time(const double milliseconds);
			double get_integration_time();
			std::string get_imager_type();
			void set_internal_trigger();
			void set_external_trigger();
			bool is_trigger_external();
		protected:
			XCameraImplementation * pimpl;
	};
} // end namespace Resonon
#endif //end ifndef __GUARD_RESONON_IMAGER_X_H
