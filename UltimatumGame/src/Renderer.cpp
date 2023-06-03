#include "Renderer.h"
#include <cstdint>
#include <iostream>
#include <algorithm>

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	m_Image = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	m_Players = std::vector<Player>(width * height);

	GenerateImageData();	
}

void Renderer::CalculatePayoff()
{
	m_Payoff = std::vector<float>(m_Players.size());

	size_t width = m_Image->GetWidth();
	size_t height = m_Image->GetHeight();

	for (int32_t i = 0; i < width * height; ++i) {
			
		auto p = &m_Players[i];
		m_Payoff[i] = 0.f;

		for (const auto neighbourIndex : GetNeighboursIndex(i))
			m_Payoff[i] += p->PlayFullRound(m_Players[neighbourIndex]);
	}
}

void Renderer::SetBestPlayer()
{
	m_BestPlayerIndex = std::distance(m_Payoff.begin(), std::max_element(m_Payoff.begin(), m_Payoff.end()));
}

//Tournament selection against neighbour
void Renderer::MutatePlayers()
{
	for (size_t i = 0; i < EvolutionSpeed; ++i) {
		size_t index1 = Walnut::Random::UInt() % m_Players.size();
		size_t index2 = GetNeighboursIndex(i)[Walnut::Random::UInt() % 4];

		size_t best = (m_Payoff[index1] > m_Payoff[index2]) ? index1 : index2;
		size_t worst = (m_Payoff[index1] <= m_Payoff[index2]) ? index1 : index2;

		m_Players[worst] = m_Players[best];

		//Mutation
		auto rand = Walnut::Random::Float() * 0.01f - 0.005f;
		m_Players[worst].p = std::max(m_Players[worst].p + rand, 0.f);
		rand = Walnut::Random::Float() * 0.01f - 0.005f;
		m_Players[worst].q = std::max(m_Players[worst].q + rand, 0.f);

	}
	GenerateImageData();
}

void Renderer::GenerateImageData()
{
	m_ImageData = std::vector<uint32_t>(m_Image->GetWidth() * m_Image->GetHeight());

	CalculatePayoff();
	SetBestPlayer();

	if (VisualizationType == 0) {
		for (size_t i = 0; i < m_ImageData.size(); ++i) {
			uint32_t color = 0xFF000000 | ((uint32_t)((m_Payoff[i] / m_Payoff[m_BestPlayerIndex]) * 255) << 8);
			m_ImageData[i] = color;
		}
	}
	else {
		for (size_t i = 0; i < m_ImageData.size(); ++i) {
			
			uint32_t color = 0xFF000000;
			const auto player = m_Players[i];

			if (player.p < 0.1f && player.q < 0.1f) color = ImGui::ColorConvertFloat4ToU32(ColorsStrategy[0]);
			else if (player.p >= 0.4f && player.q >= 0.4f) color = ImGui::ColorConvertFloat4ToU32(ColorsStrategy[2]);
			else color = ImGui::ColorConvertFloat4ToU32(ColorsStrategy[1]);
			m_ImageData[i] = color;
		}
	}

	m_Image->SetData(m_ImageData.data());
}

std::array<size_t, 4> Renderer::GetNeighboursIndex(size_t index) const
{
	const auto width = m_Image->GetWidth();
	const auto height = m_Image->GetHeight();
	int32_t x = index % width;
	int32_t y = index / width;

	return {
		(x - 1) % width + y * width,
		(x + 1) % width + y * width,
		x + ((y - 1) % height) * width,
		x + ((y + 1) % height) * width
	};
}
