#include "MetasoundNodes.h"

using namespace Metasound;

const FVertexInterface& FSigmoidOperator::DeclareVertexInterface()
{
	static const FVertexInterface Interface = 
	{
		apply([](const auto&... Verts){ return FInputVertexInterface(TInputDataVertex(Verts)...); }, as_tuple(InputVerts)),
		FOutputVertexInterface(TOutputDataVertex(OutputVertex))
	};
	return Interface;
}

const FNodeClassMetadata& FSigmoidOperator::GetNodeInfo()
{
	static const FNodeClassMetadata Metadata =
	{
		.ClassName = FNodeClassName{"UE", "Sigmoid", "Audio"},
		.DisplayName = INVTEXT("Sigmoid: 2π * arctan(x / midpoint)"),
		.DefaultInterface = DeclareVertexInterface(),
	};
	return Metadata;
}

void FSigmoidOperator::BindInputs(FInputVertexInterfaceData& InVertexData)
{
	for (const FFloatReadRef* ReadRef = &InputX; const TInputDataVertex<float>& InVert : InputVerts)
		InVertexData.BindReadVertex(InVert.VertexName, *(ReadRef++));
}

void FSigmoidOperator::BindOutputs(FOutputVertexInterfaceData& InVertexData)
{
	InVertexData.BindWriteVertex(OutputVertex.VertexName, Result);
}

TUniquePtr<IOperator> FSigmoidOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults)
{
	auto Inputs = views::transform(InputVerts, [&](const TInputDataVertex<float>& InputVertex) 
		{ return InParams.InputData.GetOrCreateDefaultDataReadReference<float>(InputVertex.VertexName, InParams.OperatorSettings); });
	return MakeUnique<FSigmoidOperator>(begin(Inputs));
}

void FSigmoidOperator::Execute()
{
	if (*InputFloor != 0)
		*Result = sigmoid(*InputX, *InputCeiling, *InputFloor, *InputMidpoint);
	else
		*Result = sigmoid(*InputX, *InputMidpoint) * *InputCeiling;
}

class FSigmoidNode final : public FNodeFacade
{
public:
	FSigmoidNode(const FNodeInitData& InitData)
		: FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSigmoidOperator>()) {}
};

METASOUND_REGISTER_NODE(FSigmoidNode);