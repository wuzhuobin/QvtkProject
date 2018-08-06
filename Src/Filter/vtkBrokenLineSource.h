#ifndef __VTK_BROKEN_LINE_SOURCE_H__
#define __VTK_BROKEN_LINE_SOURCE_H__
#pragma once
#include <vtkLineSource.h>
class vtkBrokenLineSource : public vtkLineSource
{
public:
	static vtkBrokenLineSource *New();
	vtkTypeMacro(vtkBrokenLineSource, vtkLineSource);
	virtual void PrintSelf(ostream &os, vtkIndent vtkIndent) override;
	vtkGetMacro(Solid, unsigned int);
	vtkSetMacro(Solid, unsigned int);
	vtkGetMacro(Empty, unsigned int);
	vtkSetMacro(Empty, unsigned int);
protected:
	vtkBrokenLineSource();
	virtual ~vtkBrokenLineSource() override;
	virtual int RequestData(vtkInformation *info, vtkInformationVector **input, vtkInformationVector *output) override;
	unsigned int Solid;
	unsigned int Empty;
private:
	vtkBrokenLineSource(const vtkLineSource&) = delete;
	void operator=(const vtkLineSource&) = delete;
};
#endif // !__VTK_BROKEN_LINE_SOURCE_H__
