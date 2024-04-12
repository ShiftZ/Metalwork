#pragma once

using namespace Metasound;

class FSigmoidOperator final : public TExecutableOperator<FSigmoidOperator>
{
public:
	FSigmoidOperator(auto In) :
		InputX(*In++), InputCeiling(*In++), InputFloor(*In++), InputMidpoint(*In++), Result(FFloatWriteRef::CreateNew()) { Execute(); }

	static const FVertexInterface& DeclareVertexInterface();
	static const FNodeClassMetadata& GetNodeInfo();

	void BindInputs(FInputVertexInterfaceData& InVertexData) override;
	void BindOutputs(FOutputVertexInterfaceData& InVertexData) override;

	void Execute();

	static TUniquePtr<IOperator> CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults);

private:
	inline static const TOutputDataVertex<float> OutputVertex = {L"Result", {INVTEXT("Result"), {}}};
	inline static const TInputDataVertex<float> InputVerts[] =
	{
		{L"X", {INVTEXT("X"), {}}},
		{L"Ceiling", {INVTEXT("Ceiling"), {}}, 1.f},
		{L"Floor", {INVTEXT("Floor"), {}}},
		{L"Midpoint", {INVTEXT("Midpoint"), {}}, 1.f},
	};

	FFloatReadRef InputX, InputCeiling, InputFloor, InputMidpoint;
	FFloatWriteRef Result;
};