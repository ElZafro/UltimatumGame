#pragma once

#include "Walnut/Image.h"
#include "Walnut/Random.h"

#include <memory>
#include <vector>
#include <array>
#include <imgui.h>
#include <unordered_set>

inline float ProbabilityW = 0.f;

struct Player {
	float p, q;
	float reputation = 1.f;
	
	//p + q <= 1
	Player() : p(Walnut::Random::Float()), q((1.f - p) * Walnut::Random::Float()) {}

	float PlayFullRound(const Player& other) {

		float payoff = 0.f;
		//Receiver
		const auto hisOffer = (Walnut::Random::Float() <= ProbabilityW) ? std::min(other.p, reputation) : other.p;
		payoff += (hisOffer >= q) ? hisOffer : 0;
		//Offerer
		const auto myOffer = (Walnut::Random::Float() <= ProbabilityW) ? std::min(p, other.reputation) : p;
		payoff += (myOffer >= other.q) ? 1 - myOffer : 0;

		if (hisOffer >= q) 
			reputation = std::min(reputation, hisOffer);

		return payoff;
	}
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	void OnResize(uint32_t width, uint32_t height);
	void MutatePlayers();

	std::shared_ptr<Walnut::Image> GetImage() const { return m_Image; }
	const std::vector<float>* GetPayoff() const { return &m_Payoff; }
	std::pair<Player, float> GetBestPlayer() const { return std::make_pair(m_Players[m_BestPlayerIndex], m_Payoff[m_BestPlayerIndex]); };

public:
	int PlayerSizeInPixels = 24;
	int EvolutionSpeed = 10;
	int VisualizationType = 0;
	std::array<ImVec4, 4>	ColorsStrategy = { 
		ImVec4(244.0f / 255.0f, 203.0f / 255.0f, 229.0f / 255.0f, 1.f),
		ImVec4(185.0f / 255.0f, 194.0f / 255.0f, 237.0f / 255.0f, 1.f),
		ImVec4(226.0f / 255.0f, 245.0f / 255.0f, 223.0f / 255.0f, 1.f)
	};

private:
	void CalculatePayoff();
	void SetBestPlayer();
	void GenerateImageData();
	std::array<size_t, 4> GetNeighboursIndex(size_t index) const;

private:
	std::shared_ptr<Walnut::Image> m_Image;
	std::vector<uint32_t> m_ImageData;
	std::vector<Player> m_Players;
	std::vector<float> m_Payoff;
	std::unordered_set<uint32_t> m_PlayersKnowReputation;

	size_t m_BestPlayerIndex = 0;
};