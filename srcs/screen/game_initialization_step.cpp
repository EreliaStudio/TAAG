#include "screen/initialization_screen.hpp"

#include "structure/context.hpp"

InitialisationScreen::GameInitializationStep::GameInitializationStep(TextArea& p_textArea) :
	_textArea(p_textArea)
{
	
}

void InitialisationScreen::GameInitializationStep::onStart()
{
    ChunkRenderer::setSpriteSheet(
        AssetAtlas::instance()->spriteSheet(L"ChunkSpriteSheet"));

    spk::JSON::File cfg(L"resources/world_config.json");
    const auto &nodes = cfg[L"Nodes"].asArray();

    for (const spk::JSON::Object *nodePtr : nodes)
    {
        const auto &j = *nodePtr;

        const size_t id = j[L"ID"].as<long>();
        const Node::Type type = toNodeType(j[L"Type"].as<std::wstring>());

        spk::Vector2Int spritePos = j.contains(L"Sprite") ? j[L"Sprite"] : spk::Vector2Int{0,0};

        spk::Vector2Int animOffset = j.contains(L"AnimationOffset") ? j[L"AnimationOffset"] : spk::Vector2Int{0,0};
			
        Node::Flag flags = Node::Flag::None;
        if (j.contains(L"Flag"))
        {
            const auto &flagField = j[L"Flag"];
            if (flagField.isArray())
			{
                for (auto *f : flagField.asArray())
				{
					flags |= toNodeFlag(f->as<std::wstring>());
				}
			}
            else
			{
                flags = toNodeFlag(flagField.as<std::wstring>());
			}
        }

        const long frameDuration = j.contains(L"FrameDuration") ? j[L"FrameDuration"].as<long>() : 0;
        const size_t nbFrames = j.contains(L"NbFrame") ? j[L"NbFrame"].as<long>() : 1;

        Context::instance()->nodeMap.addNode(id, Node(flags, type, spritePos, animOffset, frameDuration, nbFrames));
    }
}

void InitialisationScreen::GameInitializationStep::onPending()
{
	finish();
}

spk::StateMachine::Step::ID InitialisationScreen::GameInitializationStep::onFinish()
{
	_textArea.setText(L"Nodemap initialization complete !");
	return InitializationClosureStep::Name;
}
