#pragma once

#include <memory>

#include "Parameters.h"
#include "watermark/watermarkbase.h"
#include "io/InputManagerBase.h"
#include "io/OutputManagerBase.h"

/**
 * @brief Main class.
 *
 */
class WatermarkManager
{
	private:
		const Parameters& conf;

	public:
		// Ajouter data : bits à encoder
		Input_p _input = nullptr;
		Output_p _output = nullptr;
		Watermark_p _watermark = nullptr;


		WatermarkManager(const Parameters& parameters):
			conf(parameters)
		{

		}
		WatermarkManager(const WatermarkManager& sm):
			conf(sm.conf)
		{
			_watermark.reset(sm._watermark->clone());
		}

		const WatermarkManager& operator=(const WatermarkManager& sm)
		{
			_watermark.reset(sm._watermark->clone());

			return *this;
		}

		void onDataUpdate()
		{
			_watermark->onDataUpdate();
		}

		void execute()
		{
			while(IData* buf = _input->getNextBuffer())
			{
				(*_watermark)(buf);

				_output->writeNextBuffer(buf);
				delete buf;
			}
		}






};