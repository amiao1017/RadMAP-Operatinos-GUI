/*
 * Resonon API
 *
 * By using this API, the user agrees to the terms and conditions as stated in the
 * document "Resonon API Terms of Use", located on the Resonon website
 * at: http://www.resonon.com/downloads/Resonon_API_Terms_of_Use.pdf.
 *
 */


#ifndef __GUARD_RESONON_IMAGER_PGR_H
#define __GUARD_RESONON_IMAGER_PGR_H
#include "resonon_imager_base.h"
#include <string>

namespace Resonon
{
	class PGRCameraImplementation;

	/**
	 * An abstract base class for imagers based on Point Grey Cameras.
	 */
	class RESONONDLL PikaPGRBase : public ResononImagerBase
	{
		public:
			PikaPGRBase();
			virtual ~PikaPGRBase();
			void set_gain(const double gain);
			double get_gain();
			double get_min_gain();
			double get_max_gain();
			virtual void connect()=0;
			void disconnect();
			virtual int get_frame_buffer_size_in_bytes();
			unsigned short* get_frame(unsigned short* buffer);
			void start_frame_grabbing();
			void stop_frame_grabbing();
			virtual void set_window_bounds(int start_band,
										   int end_band,
										   int start_sample,
										   int end_sample);
			int get_band_count();
			int get_sample_count();
			int get_window_start_band();
			int get_window_end_band();
			int get_window_start_sample();
			int get_window_end_sample();
			void set_framerate(const double frames_per_second);
			double get_framerate();
			double get_min_integration_time();
			double get_max_integration_time();
			double get_min_framerate();
			double get_max_framerate();
			void set_integration_time(const double milliseconds);
			double get_integration_time();
			void set_internal_trigger();
			void set_external_trigger();
			bool is_trigger_external();
		protected:
			PGRCameraImplementation * pimpl;
	};

	/**
	 * Interface to <a href=http://www.resonon.com/imagers_pika_iii.html>Pika IIi hyperspectral imagers
	 * </a> with firewire 1394b connectivity.
	 */
	class RESONONDLL Pika2I : public PikaPGRBase
	{
		public:
			Pika2I();
			void connect();
			std::string get_imager_type();
	};

	/**
	 * Interface to <a href=http://www.resonon.com/imagers_pika_xc.html>Pika XC hyperspectral imagers</a> with firewire
	 * 1394b connectivity. This interface is for the standard camera typically installed on a Pika XC. The Pika XC is
	 * compatible with any 2/3" C-mount camera; should another camera be used, another interface will be required.
	 */
	class RESONONDLL PikaXC : public PikaPGRBase
	{
		public:
			PikaXC();
			void connect();
			std::string get_imager_type();
	};

	/**
	 * Interface to <a href=http://www.resonon.com/imagers_pika_iii.html>Pika IIG hyperspectral imagers
	 * </a> with GigE connectivity. Please see \ref gige_imagers for special considerations for GigE-connected cameras.
	 *
	 */
	class RESONONDLL Pika2G : public PikaPGRBase
	{
		public:
			Pika2G();
			void connect();
			void set_window_bounds(int start_band,
								   int end_band,
								   int start_sample,
								   int end_sample);
			void set_packet_size(const int packet_size);
			int get_packet_size();
			void set_packet_delay(const int packet_delay);
			int get_packet_delay();
			int get_frame_buffer_size_in_bytes();
			std::string get_imager_type();
	};

} // end namespace Resonon
#endif //end ifndef __GUARD_RESONON_IMAGER_PGR_H

