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
		.DisplayName = INVTEXT("Sigmoid: x / sqrt(1 + x^2)"),
		.DefaultInterface = DeclareVertexInterface(),
	};
	return Metadata;
}

FDataReferenceCollection FSigmoidOperator::GetInputs() const
{
	FDataReferenceCollection InputDataReferences;
	for (const FFloatReadRef* ReadRef = &InputX; const TInputDataVertex<float>& InVert : InputVerts)
		InputDataReferences.AddDataReadReference(InVert.VertexName, *(ReadRef++));
	return InputDataReferences;
}

FDataReferenceCollection FSigmoidOperator::GetOutputs() const
{
	FDataReferenceCollection OutputDataReferences;
	OutputDataReferences.AddDataReadReference(OutputVertex.VertexName, Result);
	return OutputDataReferences;
}

TUniquePtr<IOperator> FSigmoidOperator::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
{
	auto Inputs = views::transform(InputVerts, [&](const TInputDataVertex<float>& InputVertex) 
	{
		return InParams.InputDataReferences.GetDataReadReferenceOrConstructWithVertexDefault<float>
			(DeclareVertexInterface().GetInputInterface(), InputVertex.VertexName, InParams.OperatorSettings);
	});

	return MakeUnique<FSigmoidOperator>(begin(Inputs));
}

void FSigmoidOperator::Execute()
{
	if (*InputFloor != 0)
		*Result = sigmoid(*InputX, *InputCeiling, *InputFloor, *InputMidpoint);
	else
		*Result = sigmoid(*InputX, *InputMidpoint) * *InputCeiling;
}

class FTutorialNode final : public FNodeFacade
{
public:
	FTutorialNode(const FNodeInitData& InitData)
		: FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSigmoidOperator>()) {}
};

METASOUND_REGISTER_NODE(FTutorialNode);

