/*
 * Resonon API
 *
 * By using this API, the user agrees to the terms and conditions as stated in the
 * document "Resonon API Terms of Use", located on the Resonon website
 * at: http://www.resonon.com/downloads/Resonon_API_Terms_of_Use.pdf.
 *
 */


#ifndef __GUARD_RESONON_IMAGER_BASE_H
#define __GUARD_RESONON_IMAGER_BASE_H

#ifdef RESONON_BUILD_DLL
    #define RESONONDLL __declspec(dllexport)
#else
    #define RESONONDLL __declspec(dllimport)
#endif

#include <string>

/**
 * The Resonon namespace contains all public classes.
 */
namespace Resonon
{
	/**
	 * An abstract base class which provides a common interface for all imagers.
	 */
	class RESONONDLL ResononImagerBase
	{
		public:
			ResononImagerBase();
			virtual ~ResononImagerBase();
			double get_slope();
			double get_intercept();
			void set_slope(const double slope);
			void set_intercept(const double intercept);

			double get_wavelength_at_band(const int band);
			int get_nearest_band_to_wavelength(const double wavelength);

			virtual void connect()=0;
			virtual void disconnect()=0;
			virtual int get_frame_buffer_size_in_bytes()=0;
			virtual unsigned short* get_frame(unsigned short* buffer)=0;
			virtual void start_frame_grabbing()=0;
			virtual void stop_frame_grabbing()=0;
			virtual void set_framerate(const double frames_per_second)=0;
			virtual void set_window_bounds(int start_band,
										   int end_band,
										   int start_sample,
										   int end_sample)=0;
			virtual int get_band_count()=0;
			virtual int get_sample_count()=0;
			virtual int get_window_start_band()=0;
			virtual int get_window_end_band()=0;
			virtual int get_window_start_sample()=0;
			virtual int get_window_end_sample()=0;
			virtual double get_framerate()=0;
			virtual double get_min_integration_time()=0;
			virtual double get_max_integration_time()=0;
			virtual double get_min_framerate()=0;
			virtual double get_max_framerate()=0;
			virtual void set_integration_time(const double milliseconds)=0;
			virtual double get_integration_time()=0;
			virtual std::string get_imager_type()=0;
			virtual void set_internal_trigger();
			virtual void set_external_trigger();
			virtual bool is_trigger_external();

		protected:
			double calibration_slope;
			double calibration_intercept;
			int spectral_bin;
			const double NOT_SET_VALUE_DOUBLE;
	};
} //end namespace Resonon
#endif  //end ifndef __GUARD_RESONON_IMAGER_BASE_H
