#pragma once
#include <fftw3.h>

#include "fftmanager.h"

template <typename data_type>
class FFTWManager : public FFTManager<data_type>
{
	public:
		FFTWManager(const Parameters<data_type>& cfg):
			FFTManager<data_type>(cfg)
		{
			num_instances++;
			updateSize();
		}

		FFTWManager(const FFTWManager<data_type>& orig) = delete;

		const FFTWManager<data_type>& operator=(const FFTWManager<data_type>& orig) = delete;

		virtual ~FFTWManager()
		{
			if(plan_fw) fftw_destroy_plan(plan_fw);
			if(plan_bw) fftw_destroy_plan(plan_bw);

			if(!num_instances--)
				fftw_cleanup();
		}

		virtual void forward() const override
		{
			fftw_execute(plan_fw);
		}

		virtual void backward() const override
		{
			fftw_execute(plan_bw);
		}

		virtual double normalizationFactor() const override
		{
			return 1.0 / this->conf.bufferSize;
		}

		virtual void updateSize() override
		{
			if(plan_fw) fftw_destroy_plan(plan_fw);
			if(plan_bw) fftw_destroy_plan(plan_bw);

			// Initialize the fftw plans
			plan_fw = fftw_plan_dft_r2c_1d((int)this->conf.bufferSize,
										   this->_in.data(),
										   reinterpret_cast<fftw_complex*>(this->_spectrum.data()),
										   FFTW_ESTIMATE);
			plan_bw = fftw_plan_dft_c2r_1d((int)this->conf.bufferSize,
										   reinterpret_cast<fftw_complex*>(this->_spectrum.data()),
										   this->_out.data(),
										   FFTW_ESTIMATE);
		}

	private:
		fftw_plan plan_fw = nullptr;
		fftw_plan plan_bw = nullptr;

		static unsigned int num_instances;
};

template<>
unsigned int FFTWManager<double>::num_instances = 0;
