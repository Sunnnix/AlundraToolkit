#include "AlundraAudioPlayer.h"
#include <vector>
#include <fstream>
#include <string>
#include <format>
#include <iostream>
#include "Audio/AudioEngine.h"

namespace Alundra {
	namespace AudioPlayer {

        void loadSOUND_BIN();

        enum SectionType {
            SEQUENCE, VAB, UNKNOWN
        };

        struct SectionInfo {
            SectionType type;       // "pQES" oder "pBAV"
            size_t offset;          // Offset in Buffer
            size_t size;            // Size in Bytes
        };

        struct Track {
            std::string name;
            int seq;
            int vab;
            int loops;

            Track(std::string name, int seq, int vab, int loops = 0) :
                name(name),
                seq(seq),
                vab(vab),
                loops(loops) {
            }
        };

        bool running;

		std::vector<uint8_t*> sequences{};
		std::vector<VAB::VAB*> vabs{};

        bool tryParse(std::string& input, int& output) {
            try {
                output = std::stoi(input);
            } catch (std::invalid_argument) {
                return false;
            }
            return true;
        }

		void Start() {
            loadSOUND_BIN();

            if (sequences.size() == 0) {
                printf("No sequences found!\n");
                return;
            }
            if (vabs.size() == 0) {
                printf("No vabs found!\n");
                return;
            }

            std::vector<Track> tracks{};
            //tracks.emplace_back(std::format("{} - SE", tracks.size()), 0, 72);
            //tracks.emplace_back(std::format("{} - SE", tracks.size()), 1, 73);
            //tracks.emplace_back(std::format("{} - SE", tracks.size()), 2, 74);
            //tracks.emplace_back(std::format("{} - SE", tracks.size()), 3, 75);
            //tracks.emplace_back(std::format("{} - SE", tracks.size()), 4, 76);
            //tracks.emplace_back(std::format("{} - SE", tracks.size()), 5, 77);
            tracks.emplace_back(std::format("{} - Title", tracks.size()), 6, 78);
            tracks.emplace_back(std::format("{} - Jess", tracks.size()), 7, 79);
            tracks.emplace_back(std::format("{} - Panik on the Ship", tracks.size()), 8, 80);
            tracks.emplace_back(std::format("{} - Boss 1", tracks.size()), 9, 81);
            tracks.emplace_back(std::format("{} - Boss 2", tracks.size()), 10, 82);
            tracks.emplace_back(std::format("{} - Lars' Crypt", tracks.size()), 11, 83);
            tracks.emplace_back(std::format("{} - The Wind That Shock the Earth", tracks.size()), 12, 84);
            tracks.emplace_back(std::format("{} - Burning Village", tracks.size()), 13, 85);
            tracks.emplace_back(std::format("{} - Lizardman's Lair", tracks.size()), 14, 86);
            tracks.emplace_back(std::format("{} - Requiem", tracks.size()), 15, 87);
            tracks.emplace_back(std::format("{} - Torla", tracks.size()), 16, 88);
            tracks.emplace_back(std::format("{} - Church", tracks.size()), 17, 89);
            tracks.emplace_back(std::format("{} - Nirude", tracks.size()), 18, 90);
            tracks.emplace_back(std::format("{} - Melzas Battle 2", tracks.size()), 19, 91);
            tracks.emplace_back(std::format("{} - Ronan", tracks.size()), 20, 92);
            tracks.emplace_back(std::format("{} - Village Inoa", tracks.size()), 21, 93);
            tracks.emplace_back(std::format("{} - Murgg Woods", tracks.size()), 22, 94);
            tracks.emplace_back(std::format("{} - The House of Taran", tracks.size()), 23, 95);
            tracks.emplace_back(std::format("{} - Zazan Boss", tracks.size()), 24, 96);
            tracks.emplace_back(std::format("{} - Mirude Boss", tracks.size()), 25, 97);
            tracks.emplace_back(std::format("{} - Cave", tracks.size()), 26, 98);
            tracks.emplace_back(std::format("{} - Desert", tracks.size()), 27, 99);
            tracks.emplace_back(std::format("{} - Lake Shrine", tracks.size()), 28, 100);
            tracks.emplace_back(std::format("{} - New Adventure?", tracks.size()), 29, 101);
            tracks.emplace_back(std::format("{} - On the Ship", tracks.size()), 30, 102);
            tracks.emplace_back(std::format("{} - Event 1", tracks.size()), 31, 103);
            tracks.emplace_back(std::format("{} - Klines Dream", tracks.size()), 32, 104);
            tracks.emplace_back(std::format("{} - Underwater", tracks.size()), 33, 105);
            tracks.emplace_back(std::format("{} - Pub", tracks.size()), 34, 106);
            tracks.emplace_back(std::format("{} - Nightmare", tracks.size()), 35, 107);
            tracks.emplace_back(std::format("{} - Dream a Dream", tracks.size()), 36, 108);
            tracks.emplace_back(std::format("{} - Twin Dream", tracks.size()), 37, 109);
            tracks.emplace_back(std::format("{} - Melzas Battle 1", tracks.size()), 38, 110);
            tracks.emplace_back(std::format("{} - Mimings", tracks.size()), 39, 111);
            tracks.emplace_back(std::format("{} - Fairy Pond / Meia's Dream", tracks.size()), 40, 112);
            tracks.emplace_back(std::format("{} - Unknown Fanfare", tracks.size()), 41, 113, 1);
            tracks.emplace_back(std::format("{} - Holy Sword Fanfare", tracks.size()), 42, 114, 1);
            tracks.emplace_back(std::format("{} - Mystic 2", tracks.size()), 43, 115, 1);
            tracks.emplace_back(std::format("{} - Game Over", tracks.size()), 44, 116);
            tracks.emplace_back(std::format("{} - Special Item", tracks.size()), 45, 117, 1);
            tracks.emplace_back(std::format("{} - File Select", tracks.size()), 46, 118);
            tracks.emplace_back(std::format("{} - Boss Fanfare", tracks.size()), 47, 119, 1);
            tracks.emplace_back(std::format("{} - Mystic 1", tracks.size()), 48, 120, 1);
            tracks.emplace_back(std::format("{} - ", tracks.size()), 49, 121, 1);
            tracks.emplace_back(std::format("{} - Old Man", tracks.size()), 50, 122);
            tracks.emplace_back(std::format("{} - Old Man 2", tracks.size()), 51, 123);

            AudioEngine::Init();

            running = true;
            while (running) {
                int seq = 0;
                int vab = 0;
                int loops = 0;

                std::string input;
                printf("Mix SEQ and VAB or use Track? (M/T)\n");
                getline(std::cin, input);
                if (input.empty())
                    continue;
                if (input[0] == 'M' || input[0] == 'm') {
                    // Mix track
                    printf("There are %d sequences. Which one to use?\n", sequences.size());
                    getline(std::cin, input);
                    while (!tryParse(input, seq)) {
                        printf("Bad entry. Enter a NUMBER: ");
                        getline(std::cin, input);
                    }
                    printf("There are %d vabs. Which one to use?\n", vabs.size());
                    getline(std::cin, input);
                    while (!tryParse(input, vab)) {
                        printf("Bad entry. Enter a NUMBER: ");
                        getline(std::cin, input);
                    }
                } else if (input[0] == 'T' || input[0] == 't') {
                    while (true) {
                        printf("Which track should it be?\n");
                        for (auto& track : tracks)
                            printf((track.name + "\n").c_str());
                        getline(std::cin, input);
                        int trackID;
                        if (!tryParse(input, trackID) || trackID < 0 || trackID >= tracks.size())
                            continue;
                        auto& track = tracks[trackID];
                        seq = track.seq;
                        vab = track.vab;
                        loops = track.loops;
                        break;
                    }
                } else
                    continue;

                AudioEngine::SetSEQ(sequences[seq], loops, loops > 0 ? -1 : 0);
                if (!AudioEngine::SetSoundFont(vabs[vab])) {
                    printf("Error setting SoundFont!\n");
                    continue;
                }

                AudioEngine::Play();

                getline(std::cin, input);

                AudioEngine::Stop();
            }
		}

        void Stop() {
            running = false;
        }

        bool IsMagic(const uint8_t* ptr) {
            return std::memcmp(ptr, "pQES", 4) == 0 || std::memcmp(ptr, "pBAV", 4) == 0;
        }

        SectionType GetMagicType(const uint8_t* ptr) {
            std::string magic(reinterpret_cast<const char*>(ptr), 4);
            if (magic == "pQES")
                return SEQUENCE;
            else if (magic == "pBAV")
                return VAB;
        }

        void GenerateSectionInfos(std::ifstream& file, std::vector<SectionInfo>& sections) {

            std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            // 1. Alle Section-Starts finden
            for (size_t i = 0; i + 4 <= buffer.size(); ++i)
                if (IsMagic(&buffer[i]))
                    sections.push_back({ GetMagicType(&buffer[i]), i, 0 }); // size wird später gesetzt

            // 2. Größen berechnen
            for (size_t i = 0; i < sections.size(); ++i) {
                size_t start = sections[i].offset;
                size_t end = (i + 1 < sections.size()) ? sections[i + 1].offset : buffer.size();
                sections[i].size = end - start;
            }
        }

		void loadSOUND_BIN() {
            std::ifstream file("SOUND.BIN", std::ios::binary);
            if (!file) {
                fprintf(stderr, "SOUND.BIN couldn't be opened!\n");
                return;
            }

            std::vector<SectionInfo> sections;

            GenerateSectionInfos(file, sections);

            uint8_t* buffer;

            for (const auto& section : sections) {
                file.seekg(section.offset);
                buffer = new uint8_t[section.size];
                file.read((char*)buffer, section.size);
                switch (section.type) {
                case SEQUENCE:
                    sequences.emplace_back(buffer);
                    break;
                case VAB:
                    vabs.push_back(VAB::setupVAB(buffer, vabs.size()));
                    break;
                default:
                    break;
                }
            }
		}

	}
}