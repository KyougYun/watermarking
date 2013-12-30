#pragma once

#include "FFTProxy.h"
#include "../InputManagerBase.h"
#include "../copystyle/OutputSimple.h"
#include "../copystyle/OutputOLA.h"
#include "../../mathutils/math_util.h"

template <typename data_type>
/**
 * @brief The FFTOutputProxy class
 *
 * Réalise une short time invese fourier transform
 */
class FFTOutputProxy : public FFTProxy<data_type>, public OutputManagerBase<data_type>
{
		using IOManagerBase<data_type>::channels;
		using IOManagerBase<data_type>::frames;
		using OutputManagerBase<data_type>::copyHandler;
		using FFTProxy<data_type>::fft;
	private:
		Output_p outputImpl = nullptr;

	public:
		FFTOutputProxy(OutputManagerBase<data_type>* output, FFT_p<data_type> fft_impl, Parameters<data_type>& cfg):
			FFTProxy<data_type>(fft_impl, cfg),
			OutputManagerBase<data_type>(nullptr, cfg),
			outputImpl(output)
		{
		}

		virtual ~FFTOutputProxy() = default;

		virtual void writeNextBuffer(Audio_p& data) final override
		{
			auto& buffer = getSpectrum<data_type>(data);

			// 0. We put our buffer back in the FFT.
			fft->spectrum() = std::move(buffer);

			// 1. Perform reverse FFT
			fft->backward();

			// 2. Normalize
			auto outbuff = new CData<data_type>;
			outbuff->_data.resize(fft->output().size());

			for(auto i = 0U; i < fft->output().size(); ++i)
			{
				outbuff->_data[i].resize(fft->output()[i].size());
				std::transform(fft->output()[i].begin(),
							   fft->output()[i].end(),
							   outbuff->_data[i].begin(),
							   [this] (const data_type& x)
				{ return x * fft->normalizationFactor(); });
			}

			data.reset(outbuff);
			outputImpl->writeNextBuffer(data);
		}
};


